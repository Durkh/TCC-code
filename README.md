Projeto feito para demonstrar o protocolo [DCP](https://github.com/Durkh/DCP.git), utilizado como Trabalho de Conclusão do Curso de engenharia de computação.

Este projeto é composto por 3 módulos independentes que são conectados apenas pelo barramento DCP. Os módulos são:

- Módulo sensor: ESP32-C3 conectado aos sensores: AHT21 e ENS160 (temperatura, umidade e qualidade do ar) por I²C. Este módulo acessa os dados dos sensores, codifica-os em uma mensagem DCP genérica e envia no barramento. Além disso, para demonstar a capacidade bidirecional, ao receber uma mensagem L3 endereçada para este módulo, um LED é aceso.

![módulo sensor](results/images/pic_esp.jpg)

- Módulo agregador: RP2350 ARM conectado a um botão, 2 LEDs e um LED RGB. Este módulo analisa os dados enviados pelo módulo sensor. Dependendo dos níveis de umidade e temperatura, um LED correspondente é aceso, além disso, o LED RGB demonstra o nível de qualidade do ar por meio de uma interpolação linear entre verde e vermelho. Por fim, o botão serve para enviar manualmente a mensagem L3 para o módulo sensor.

![módulo agregador](results/images/pict_pico2.jpg)

- Módulo interface: STM32F411C4 (blackpill) conectado a um display SSD1306 e 2 displays de 7 segmentos com seus respectivos chips de driver. Este módulo imprime os dados recebidos no barramento, tendo em mente, que as mensagens endereçadas que passam pelo barramento e não são endereçadas a este módulo são ignoradas. Na prática, os valores de temperatura, umidade e qualidade do ar são mostrados no display OLED e a parcela inteira de temperatura (em Celcius) é demonstrada no display de 7 segmentos.

![módulo interface](results/images/pict_STM.jpg)

> o projeto originalmente era previsto utilizar o PIC16F45550 em vez do Pico2, entretanto, por problemas técnicos, não foi possível fazer flash do código no chip. Portanto, o desenvolvimento do código do PIC16F4550 foi interrompido após o teste no simulador do MPLAB.


