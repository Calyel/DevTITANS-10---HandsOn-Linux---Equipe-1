

# DevTITANS 10 - HandsOn Linux - Equipe 01

Bem-vindo ao repositório da Equipe 0X do HandsON de Linux do DevTITANS! Este projeto contém um firmware para o ESP32 escrito em formato Arduino `.ino`, bem como um driver do kernel Linux escrito em C. O objetivo é demonstrar como criar uma solução completa de hardware e software que integra um dispositivo ESP32 com um sistema Linux.

## Tabela de Conteúdos

- [Contribuidores](#contribuidores)
- [Introdução](#introdução)
- [Recursos](#recursos)
- [Requisitos](#requisitos)
- [Configuração de Hardware](#configuração-de-hardware)
- [Instalação](#instalação)
- [Uso](#uso)
- [Contato](#contato)

## Contribuidores

<a href="https://github.com/marcianocamposgit"> <img src="https://github.com/marcianocamposgit.png" width="180" alt="Marciano Campos">
<a href="https://github.com/Calyel"> <img src="https://github.com/Calyel.png" width="180" alt="Calyel">
<a href="https://github.com/gab-rielly"> <img src="https://github.com/gab-rielly.png" width="180" alt="Gab Rielly">
<a href="https://github.com/Kevin-Freitas"> <img src="https://github.com/Kevin-Freitas.png" width="180" alt="Kevin Freitas">
<a href="https://github.com/emersonsena"> <img src="https://github.com/emersonsena.png" width="180" alt="Emerson Sena">

  - **Marciano Campos:** Desenvolvedor de Firmware e mantenedor do projeto.
  - **Calyel Junio De Andrade Dos Santos:** Desenvolvedor de Firmware, do driver Linux e responsável pela documentação técnica.
  - **Gabrielly Ferreira Rodrigues:** Desenvolvedora do driver Linux e de Firmware.
  - **Kevin Carlos Tavares De Freitas:** Desenvolvedor de Firmware e responsável pela montagem e integração do circuito.
  - **Emerson Sena De Souza:** Desenvolvedor de Firmware e responsável pela revisão e validação do código-fonte.


## Introdução

Este projeto serve como um exemplo para desenvolvedores interessados em construir e integrar soluções de hardware personalizadas com sistemas Linux. Inclui os seguintes componentes:
- Firmware para o microcontrolador ESP32 para lidar com operações específicas do dispositivo.
- Um driver do kernel Linux que se comunica com o dispositivo ESP32, permitindo troca de dados e controle.

## Recursos

- **Firmware ESP32:**
  - Aquisição básica de dados de sensores.
  - Comunicação via Serial com o driver Linux.
  
- **Driver do Kernel Linux:**
  - Rotinas de inicialização e limpeza.
  - Operações de arquivo de dispositivo (`GET_LED`, `SET_LED`, `GET_LDR`).
  - Comunicação com o ESP32 via Serial.

## Requisitos

- **Hardware:**
  - Placa de Desenvolvimento ESP32
  - Máquina Linux
  - Protoboard e Cabos Jumper
  - Sensor LDR
  
- **Software:**
  - Arduino IDE
  - Kernel Linux 4.0 ou superior
  - GCC 4.8 ou superior
  - Make 3.81 ou superior

## Configuração de Hardware

1. **Conecte o ESP32 à sua Máquina Linux:**
    - Use um cabo USB.
    - Conecte os sensores ao ESP32 conforme especificado no firmware.

2. **Garanta a alimentação e conexões adequadas:**
    - Use um protoboard e cabos jumper para montar o circuito.
    - Consulte o diagrama esquemático fornecido no diretório `esp32` para conexões detalhadas.

## Instalação

### Firmware ESP32

1. **Abra o Arduino IDE e carregue o firmware:**
    ```sh
    Arquivo -> Abrir -> Selecione `smartlamp.ino`
    ```

2. **Configure a Placa e a Porta:**
    ```sh
    Ferramentas -> Placa -> Node32s
    Ferramentas -> Porta -> Selecione a porta apropriada
    ```

3. **Carregue o Firmware:**
    ```sh
    Sketch -> Upload (Ctrl+U)
    ```

### Driver Linux

1. **Clone o Repositório:**
    ```sh
    git clone https://github.com/seuusuario/Hands-On-Linux.git
    cd Hands-On-Linux
    ```

2. **Compile o Driver:**
    ```sh
    cd smartlamp-kernel-module
    make
    ```

3. **Carregue o Driver:**
    ```sh
    sudo insmod smartlamp.ko
    ```

4. **Verifique o Driver:**
    ```sh
    dmesg | tail
    ```

## Uso

Depois que o driver e o firmware estiverem configurados, você poderá interagir com o dispositivo ESP32 através do sistema Linux.

- **Escrever para o Dispositivo:**
    ```sh
    echo "100" > /sys/kernel/smartlamp/led
    ```

- **Ler do Dispositivo:**
    ```sh
    cat /sys/kernel/smartlamp/led
    ```

- **Verificar Mensagens do Driver:**
    ```sh
    dmesg | tail
    ```

- **Remover o Driver:**
    ```sh
    sudo rmmod smartlamp
    ```
    ## Documentação Completa

Para mais detalhes sobre o desenvolvimento do projeto, consulte a **Wiki** do repositório:

🔗 [Wiki do SmartLamp](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki)

A Wiki contém os relatórios detalhados de cada tarefa:

- [Tarefa 1 - Montar o protótipo ESP32 e firmware](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki/1-%E2%80%90-Montar-o-prototipo-ESP32-e-firmware-a-partir-das-especifica%C3%A7%C3%B5es)
- [Tarefa 2 - Compilar e rodar o driver mínimo](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki/2-%E2%80%90-Compilar-e-rodar-o-driver-m%C3%ADnimo-no-notebook)
- [Tarefa 2.1.2 - Fazer o driver ler uma mensagem pela porta serial](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki/2.1.2--%E2%80%90-Fazer-o-driver-ler-uma-mensagem-pela-porta-serial)
- [Tarefa 2.2 - Fazer o driver escrever uma mensagem pela porta serial](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki/2.2-%E2%80%90-Fazer-o-driver-escrever-uma-mensagem-pela-porta-serial)
- [Tarefa 2.3 - Sysfs no driver](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki/2.3-%E2%80%90-Sysfs-no-driver)
- [Tarefa 4 ‐ Finalizar o Repositório](https://github.com/Calyel/DevTITANS-10---HandsOn-Linux---Equipe-1/wiki/4-%E2%80%90-Finalizar-o-Reposit%C3%B3rio)

  ---
    
## Contato

Para perguntas, sugestões ou feedback, entre em contato com o mantenedor do projeto em [maintainer@example.com](mailto:maintainer@example.com).
