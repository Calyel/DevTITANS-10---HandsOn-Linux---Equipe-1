#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

MODULE_AUTHOR("DevTITANS <devtitans@icomp.ufam.edu.br>");
MODULE_DESCRIPTION("Driver de acesso ao SmartLamp (ESP32 com Chip Serial CP2102)");
MODULE_LICENSE("GPL");

#define MAX_RECV_LINE      100
#define MAX_READ_ATTEMPTS  10

#define VENDOR_ID  0x1a86
#define PRODUCT_ID 0x55d4

/* Buffer para armazenar uma linha completa recebida. */
static char recv_line[MAX_RECV_LINE];

/* Valor atual do sensor LDR. */
static int LDR_value = -1;

/* Referência para o dispositivo USB. */
static struct usb_device *smartlamp_device;

/* Endereços dos endpoints USB. */
static uint usb_in;
static uint usb_out;

/* Buffers utilizados nas transferências USB. */
static char *usb_in_buffer;
static char *usb_out_buffer;

/* Tamanho máximo de uma mensagem USB. */
static int usb_max_size;

static const struct usb_device_id id_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    { }
};

MODULE_DEVICE_TABLE(usb, id_table);

static int usb_probe(
    struct usb_interface *interface,
    const struct usb_device_id *id
);

static void usb_disconnect(struct usb_interface *interface);

static int usb_write_serial(char *cmd, int param);

static int usb_read_serial(void);

/*
 * Configura os parâmetros seriais do CP2102 usando mensagens de controle.
 */
static int smartlamp_config_serial(struct usb_device *dev)
{
    int ret;
    __le32 baudrate = cpu_to_le32(9600);

    /*
     * Placas CH9102 (1a86:55d4) usam cdc_acm e não precisam dos
     * comandos de controle do CP2102.
     */
    if (le16_to_cpu(dev->descriptor.idVendor) == VENDOR_ID) {
        printk(KERN_INFO
               "SmartLamp: Dispositivo CH9102 detectado, "
               "pulando configuração CP2102.\n");
        return 0;
    }

    printk(KERN_INFO
           "SmartLamp: Configurando a porta serial...\n");

    /*
     * Habilita a interface UART do CP2102.
     *
     * bRequest: 0x00 - CP210X_IFC_ENABLE
     * wValue:   0x0001 - UART habilitada
     */
    ret = usb_control_msg(
        dev,
        usb_sndctrlpipe(dev, 0),
        0x00,
        0x41,
        0x0001,
        0,
        NULL,
        0,
        1000
    );

    if (ret < 0) {
        printk(KERN_ERR
               "SmartLamp: Erro ao habilitar a UART: %d\n",
               ret);

        return ret;
    }

    /*
     * Configura o baud rate.
     *
     * bRequest: 0x1E - CP210X_SET_BAUDRATE
     */
    ret = usb_control_msg(
        dev,
        usb_sndctrlpipe(dev, 0),
        0x1E,
        0x41,
        0,
        0,
        &baudrate,
        sizeof(baudrate),
        1000
    );

    if (ret < 0) {
        printk(KERN_ERR
               "SmartLamp: Erro ao configurar o baud rate: %d\n",
               ret);

        return ret;
    }

    printk(KERN_INFO
           "SmartLamp: Baud rate configurado para 9600\n");

    return 0;
}

static struct usb_driver smartlamp_driver = {
    .name       = "smartlamp",
    .probe      = usb_probe,
    .disconnect = usb_disconnect,
    .id_table   = id_table,
};

module_usb_driver(smartlamp_driver);

/*
 * Executado quando o dispositivo é conectado à USB.
 */
static int usb_probe(
    struct usb_interface *interface,
    const struct usb_device_id *id
)
{
    struct usb_endpoint_descriptor *usb_endpoint_in;
    struct usb_endpoint_descriptor *usb_endpoint_out;
    int ret;

    printk(KERN_INFO
           "SmartLamp: Dispositivo conectado.\n");

    smartlamp_device = interface_to_usbdev(interface);

    /*
     * Localiza os endpoints Bulk IN e Bulk OUT.
     */
    ret = usb_find_common_endpoints(
        interface->cur_altsetting,
        &usb_endpoint_in,
        &usb_endpoint_out,
        NULL,
        NULL
    );

    if (ret) {
        printk(KERN_ERR
               "SmartLamp: Endpoints Bulk IN e OUT não encontrados: %d\n",
               ret);

        return ret;
    }

    usb_max_size = usb_endpoint_maxp(usb_endpoint_in);
    usb_in = usb_endpoint_in->bEndpointAddress;
    usb_out = usb_endpoint_out->bEndpointAddress;

    printk(KERN_INFO
           "SmartLamp: Endpoint IN = 0x%02x\n",
           usb_in);

    printk(KERN_INFO
           "SmartLamp: Endpoint OUT = 0x%02x\n",
           usb_out);

    printk(KERN_INFO
           "SmartLamp: Tamanho máximo do endpoint = %d bytes\n",
           usb_max_size);

    /*
     * Aloca os buffers usados para entrada e saída.
     */
    usb_in_buffer = kmalloc(usb_max_size, GFP_KERNEL);
    usb_out_buffer = kmalloc(usb_max_size, GFP_KERNEL);

    if (!usb_in_buffer || !usb_out_buffer) {
        printk(KERN_ERR
               "SmartLamp: Falha ao alocar os buffers USB.\n");

        kfree(usb_in_buffer);
        kfree(usb_out_buffer);

        usb_in_buffer = NULL;
        usb_out_buffer = NULL;

        return -ENOMEM;
    }

    /*
     * Configura o CP2102 antes de iniciar a comunicação.
     */
    ret = smartlamp_config_serial(smartlamp_device);

    if (ret < 0) {
        printk(KERN_ERR
               "SmartLamp: Falha na configuração da serial.\n");

        kfree(usb_in_buffer);
        kfree(usb_out_buffer);

        usb_in_buffer = NULL;
        usb_out_buffer = NULL;

        return ret;
    }

    /*
     * O firmware envia periodicamente:
     *
     * RES GET_LDR X\n
     */
    ret = usb_read_serial();

    if (ret >= 0) {
        printk(KERN_INFO
               "SmartLamp: Valor do LDR recebido: %d\n",
               LDR_value);
    } else {
        printk(KERN_ERR
               "SmartLamp: Não foi possível ler o valor do LDR.\n");
    }

    /*
     * Mesmo que a primeira leitura falhe, o driver continua associado
     * ao dispositivo.
     */
    return 0;
}

/*
 * Executado quando o dispositivo é desconectado da USB.
 */
static void usb_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO
           "SmartLamp: Dispositivo desconectado.\n");

    kfree(usb_in_buffer);
    kfree(usb_out_buffer);

    usb_in_buffer = NULL;
    usb_out_buffer = NULL;
    smartlamp_device = NULL;
}

/*
 * Envia um comando para o dispositivo USB.
 *
 * Exemplos:
 *
 * usb_write_serial("SET_LED", 80);
 * usb_write_serial("GET_LDR", 0);
 */
static int usb_write_serial(char *cmd, int param)
{
    int ret;
    int actual_size;
    int command_size;

    if (!smartlamp_device || !usb_out_buffer) {
        printk(KERN_ERR
               "SmartLamp: Dispositivo ou buffer de saída inválido.\n");

        return -1;
    }

    command_size = snprintf(
        usb_out_buffer,
        usb_max_size,
        "%s %d\n",
        cmd,
        param
    );

    if (command_size < 0 || command_size >= usb_max_size) {
        printk(KERN_ERR
               "SmartLamp: Comando maior que o buffer USB.\n");

        return -1;
    }

    printk(KERN_INFO
           "SmartLamp: Enviando comando: %s",
           usb_out_buffer);

    ret = usb_bulk_msg(
        smartlamp_device,
        usb_sndbulkpipe(smartlamp_device, usb_out),
        usb_out_buffer,
        command_size,
        &actual_size,
        1000
    );

    if (ret) {
        printk(KERN_ERR
               "SmartLamp: Erro ao enviar comando: %d\n",
               ret);

        return -1;
    }

    printk(KERN_INFO
           "SmartLamp: Comando enviado com sucesso. Bytes enviados: %d\n",
           actual_size);

    return 0;
}

/*
 * Lê uma linha completa da porta serial.
 *
 * A mensagem pode chegar fragmentada em várias transferências USB.
 *
 * Exemplo:
 *
 * Primeira leitura: "RES GET"
 * Segunda leitura:  "_LDR 45"
 * Terceira leitura: "0\n"
 *
 * Resultado final:
 *
 * "RES GET_LDR 450\n"
 */
static int usb_read_serial(void)
{
    int ret;
    int actual_size;
    int recv_size = 0;
    int attempt;
    int i;
    int value;

    if (!smartlamp_device || !usb_in_buffer) {
        printk(KERN_ERR
               "SmartLamp: Dispositivo ou buffer de entrada inválido.\n");

        return -1;
    }

    printk(KERN_INFO
           "SmartLamp: Aguardando resposta do dispositivo...\n");

    memset(recv_line, 0, sizeof(recv_line));

    /*
     * Repete usb_bulk_msg até reconstruir uma linha "RES GET_LDR X".
     * Os dados podem chegar fragmentados em várias leituras.
     */
    for (attempt = 0; attempt < MAX_READ_ATTEMPTS; attempt++) {
        actual_size = 0;

        ret = usb_bulk_msg(
            smartlamp_device,
            usb_rcvbulkpipe(smartlamp_device, usb_in),
            usb_in_buffer,
            usb_max_size,
            &actual_size,
            2000
        );

        if (ret) {
            if (ret == -ETIMEDOUT) {
                continue;
            }

            printk(KERN_ERR
                   "SmartLamp: Erro na leitura USB na tentativa %d: %d\n",
                   attempt + 1,
                   ret);

            continue;
        }

        if (actual_size == 0) {
            continue;
        }

        printk(KERN_INFO
               "SmartLamp: Tentativa %d recebeu %d bytes.\n",
               attempt + 1,
               actual_size);

        for (i = 0; i < actual_size; i++) {
            if (recv_size >= MAX_RECV_LINE - 1) {
                recv_line[MAX_RECV_LINE - 1] = '\0';

                printk(KERN_ERR
                       "SmartLamp: A mensagem ultrapassou o limite "
                       "de %d caracteres.\n",
                       MAX_RECV_LINE - 1);

                return -1;
            }

            recv_line[recv_size] = usb_in_buffer[i];
            recv_size++;

            if (usb_in_buffer[i] != '\n') {
                continue;
            }

            recv_line[recv_size] = '\0';

            printk(KERN_INFO
                   "SmartLamp: Linha recebida: %s",
                   recv_line);

            ret = sscanf(recv_line, "RES GET_LDR %d", &value);

            if (ret == 1) {
                LDR_value = value;

                printk(KERN_INFO
                       "SmartLamp: LDR_value atualizado para %d\n",
                       LDR_value);

                return LDR_value;
            }

            /*
             * Linha completa, mas ainda não é a resposta esperada
             * (ex.: "SmartLamp Initialized.\n"). Limpa e continua.
             */
            printk(KERN_INFO
                   "SmartLamp: Linha ignorada, aguardando "
                   "RES GET_LDR...\n");

            recv_size = 0;
            memset(recv_line, 0, sizeof(recv_line));
        }
    }

    printk(KERN_ERR
           "SmartLamp: Não foi possível ler RES GET_LDR após %d tentativas.\n",
           MAX_READ_ATTEMPTS);

    return -1;
}