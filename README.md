# 🛠️ Projeto de Comunicação Serial com RP2040

Este repositório contém um projeto prático de comunicação serial utilizando o microcontrolador **RP2040** e a placa **BitDogLab**. O projeto envolve manipulação de **LEDs comuns e LEDs endereçáveis WS2812**, botões com interrupções e debounce, além de comunicação via **UART e I2C**.

## 📌 Funcionalidades
✅ Controle de **LEDs comuns e LEDs WS2812**
✅ Implementação de **botões com interrupção e debounce**
✅ Comunicação via **UART e I2C**
✅ Utilização do **Pico SDK** para desenvolvimento
✅ Simulação no **Wokwi**

## 🛠️ Requisitos
- Placa **BitDogLab** com **RP2040**
- Ambiente de desenvolvimento configurado com **Pico SDK**
- Simulador **Wokwi** (opcional)
- **Python** para automação de testes

## 🚀 Instalação e Uso
1️⃣ Clone este repositório:
```bash
  git clone https://github.com/seuusuario/comunicacao-rp2040.git
  cd comunicacao-rp2040
```

2️⃣ Compile o código com o **Pico SDK**:
```bash
  mkdir build && cd build
  cmake ..
  make
```

3️⃣ Envie o firmware para a placa **RP2040**:
```bash
  cp firmware.uf2 /media/usb
```

## 📝 Simulação no Wokwi:
https://wokwi.com/projects/424003535040339969

## 📩 Vídeo no Youtube:
https://youtu.be/BT4KQGSQYvA

