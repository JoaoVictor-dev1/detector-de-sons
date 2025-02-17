# Detector de Sons

## 📌 Resumo
O projeto **"Detector de Sons"** é um sistema integrado que utiliza um microfone para detectar níveis de som, acionando alarmes visuais e sonoros em resposta a detecções. O sistema é baseado na **Raspberry Pi Pico**, empregando um **display OLED** para exibir informações relevantes sobre o estado do detector e a contagem de eventos de som detectados.

## ❗ Problema
A exposição excessiva a **sons muito altos** é uma preocupação crescente em diversas áreas, como indústrias, entretenimento e ambientes urbanos. A **Organização Mundial da Saúde (OMS)** alerta que a exposição contínua a níveis de pressão sonora superiores a **85 decibéis (dB)** pode levar a danos permanentes na audição.

Além dos danos auditivos, a exposição a sons altos pode causar:
- **Estresse e Ansiedade**: Aumento nos níveis de estresse, resultando em desconforto psicológico.
- **Distúrbios do Sono**: O ruído constante pode levar a fadiga e redução da produtividade.
- **Problemas de Comunicação**: Sons excessivos dificultam a compreensão da fala, impactando interações sociais e profissionais.

Diante desse cenário, torna-se essencial desenvolver sistemas que não apenas detectem sons prejudiciais, mas também alertem os usuários sobre a necessidade de proteger sua audição.

## 🎯 Solução
O **Detector de Sons** propõe uma solução eficiente ao permitir o monitoramento de níveis sonoros em tempo real e a emissão de alertas quando o som ultrapassa limites seguros.

### 🔧 Componentes Utilizados
#### 📟 Hardware:
- **Raspberry Pi Pico**: Microcontrolador principal.
- **Microfone Analógico**: Para captura de som.
- **Display OLED (SSD1306)**: Exibição das informações.
- **LEDs (vermelho, verde, azul)**: Indicadores visuais.
- **Buzzer**: Para alarmes sonoros.
- **Botões**: Para ativação/desativação do sistema.
- **Resistores e Capacitores**: Complementares ao circuito.

#### 💻 Software:
- **Linguagem de Programação**: C
- **Bibliotecas Utilizadas**:
  - `pico/stdlib` - Biblioteca padrão da Raspberry Pi Pico.
  - `ssd1306.h` - Controle do display OLED.
  - `hardware/i2c.h` - Comunicação I2C.
  - `hardware/adc.h` - Conversão analógica-digital.

## ⚙️ Funcionamento do Sistema
### 🔄 Inicialização:
1. Configura LEDs, buzzer, botões e display OLED.
2. Configura o microfone para leitura via ADC.

### 🔘 Ativação do Sistema:
1. Pressionar o **Botão A** ativa o sistema.
2. O display OLED exibe **"Sistema ativado"**.
3. Os LEDs indicam que o sistema está ativo.
4. Pressionar o **Botão B** desativa o sistema.

### 📡 Detecção de Som:
1. Quando ativo, o microfone monitora os níveis sonoros.
2. Se o som exceder um **limite predefinido**, o display exibe **"Som detectado!"**.
3. Um **contador de eventos** é incrementado e exibido no OLED.
4. LEDs mudam de cor e o buzzer emite um **alarme sonoro**.

### 🔊 Emissão de Alarme:
- O buzzer emite um **som intermitente**.
- Uma **melodia** é tocada após o alarme.

## 📂 Estrutura do Código
O projeto está modularizado para facilitar a manutenção:
- `configure_leds()` - Inicializa LEDs e buzzer.
- `update_led_status()` - Atualiza LEDs e buzzer.
- `sound_alarm()` - Emite um alarme sonoro.
- `play_melody()` - Reproduz uma melodia no buzzer.

## 📜 Como Rodar o Projeto
1. Clone este repositório:
   ```sh
   git clone https://github.com/seu-usuario/seu-repositorio.git
   ```
2. Acesse o diretório do projeto:
   ```sh
   cd seu-repositorio
   ```
3. Compile e carregue o código na **Raspberry Pi Pico** utilizando a ferramenta apropriada.
4. Conecte os componentes conforme o esquema do circuito.
5. Ative o sistema e teste os alertas sonoros e visuais.

## 🔮 Considerações Finais
O projeto **"Detector de Sons"** é uma solução eficaz para monitoramento sonoro em diversas aplicações. Sua implementação intuitiva oferece feedback visual e sonoro ao usuário, promovendo segurança auditiva.

### 🔄 Melhorias Futuras
- **Conectividade sem fio** para notificações remotas.
- **Filtros de som** para reduzir falsos positivos.
- **Integração com IoT** para monitoramento em tempo real via rede.

## 🎥 Demonstração
Assista à demonstração do projeto no YouTube: [Clique aqui](https://youtube.com/shorts/JQMTCXoHEP8?feature=share)
