# Tarefa Prática - Comunicação Serial no RP2040

## **Objetivos**
- Compreender o funcionamento e a aplicação de comunicação serial em microcontroladores.
- Aplicar os conhecimentos adquiridos sobre UART e I2C na prática.
- Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812.
- Fixar o estudo do uso de botões de acionamento, interrupções e debounce.
- Desenvolver um projeto funcional que combine hardware e software.

## **Descrição do Projeto**
Neste projeto, você deverá utilizar os seguintes componentes conectados à placa **BitDogLab**:

- 📊 **Matriz 5x5 de LEDs (endereçáveis) WS2812** conectada à GPIO **7**.  
- 🌈 **LED RGB** com pinos conectados às GPIOs **11, 12 e 13**.  
- 🔘 **Botão A** conectado à GPIO **5**.  
- 🔘 **Botão B** conectado à GPIO **6**.  
- 🖥️ **Display SSD1306** conectado via I2C (GPIO **14** e **15**).

---

## **Funcionalidades do Projeto**

### ✅ 1. Modificação da Biblioteca `font.h`
- Adicionar caracteres minúsculos à biblioteca `font.h`.
- Use sua criatividade para criar os novos caracteres.

### ✅ 2. Entrada de Caracteres via PC
- Utilize o **Serial Monitor do VS Code** para digitar os caracteres.
- Cada caractere digitado no Serial Monitor deve ser exibido no **display SSD1306**.
  - Apenas um caractere será enviado de cada vez (não é necessário suportar o envio de strings completas).
- **Quando um número entre 0 e 9 for digitado:**
  - Um símbolo correspondente ao número deve ser exibido na **matriz 5x5 WS2812**.

### ✅ 3. Interação com o **Botão A**
- Pressionar o **Botão A** deve alternar o estado do **LED RGB Verde** (ligado/desligado).
- A operação deve ser registrada de duas formas:
  1. Uma **mensagem informativa** sobre o estado do LED deve ser exibida no **display SSD1306**.
  2. Um **texto descritivo** sobre a operação deve ser enviado ao **Serial Monitor**.

### ✅ 4. Interação com o **Botão B**
- Pressionar o **Botão B** deve alternar o estado do **LED RGB Azul** (ligado/desligado).
- A operação deve ser registrada de duas formas:
  1. Uma **mensagem informativa** sobre o estado do LED deve ser exibida no **display SSD1306**.
  2. Um **texto descritivo** sobre a operação deve ser enviado ao **Serial Monitor**.
