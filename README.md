# Carrinho 3 em 1 - Arduino mega 2560 🚗

## Equipe 🧑‍💻
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/gustavocharamba">
        <img src="https://avatars.githubusercontent.com/gustavocharamba" width="100px;" alt="Gustavo Charamba"/><br />
        <sub><b>Gustavo Charamba</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/lgss0">
        <img src="https://avatars.githubusercontent.com/lgss0" width="100px;" alt="lgss0"/><br />
        <sub><b>Luiz Gabriel</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/rafael-smoura">
        <img src="https://avatars.githubusercontent.com/rafael-smoura" width="100px;" alt="SmouraCodeX"/><br />
        <sub><b>Rafael</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/lebb8">
        <img src="https://avatars.githubusercontent.com/lebb8" width="100px;" alt="lebb8"/><br />
        <sub><b>Eduardo</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/luizmiguelbarbosa">
        <img src="https://avatars.githubusercontent.com/luizmiguelbarbosa" width="100px;" alt="Luiz Miguel Barbosa"/><br />
        <sub><b>Luiz Miguel Barbosa</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/miqueias-santos">
        <img src="https://avatars.githubusercontent.com/miqueias-santos" width="100px;" alt="Luiz Miguel Barbosa"/><br />
        <sub><b>Miqueuias Santos</b></sub>
  </tr>
</table>

## Divisão de Tarefas do Projeto ➗

<p align="center">
<table align="center">
  <tr>
    <th>Time</th>
    <th>Tarefas</th>
  </tr>
  <tr>
    <td><a href="https://github.com/gustavocharamba?tab=overview&from=2025-08-01&to=2025-08-11">Gustavo Charamba</a></td>
    <td>Programação do seguidor de linha</td>
  </tr>
  <tr>
    <td><a href="https://github.com/lgss0">lgss0</a></td>
    <td>Programação da estimativa de atrito</td>
  </tr>
  <tr>
    <td><a href="https://github.com/SmouraCodeX">SmouraCodeX</a></td>
    <td>Integração elétrica e do atrito</td>
  </tr>
  <tr>
    <td><a href="https://github.com/lebb8">lebb8</a></td>
    <td>Documentação e testes finais</td>
  </tr>
  <tr>
    <td><a href="https://github.com/luizmiguelbarbosa">luizmiguelbarbosa</a></td>
    <td>Montagem mecânica e estrutura do chassi</td>
  </tr>
  <tr>
    <td><a href="https://github.com/miqueias-santos">miqueias-santos</a></td>
    <td>Controle remoto e modos de operação</td>
  </tr>
</table>

## Modos de Operação ⚙️🛠️
Os modos são: `Seguidor de linha`, `Estimativa de Atrito` e `Controle IR`

## Bibliotecas Utilizadas 📚
```bash
Wire.h
LiquidCrystal_I2C.h
MPU9250.h
IRremote.h
AFMotor.h
```
## Conceitos Utiliados 📖
O projeto do carrinho 3 em 1 aplicou conceitos de robótica móvel e programação embarcada, incluindo o controle de motores DC por meio do Shield L293D, sensoriamento por infravermelho para detecção de linha, medição de desaceleração utilizando o acelerômetro MPU9250 e cálculo do coeficiente de atrito cinético a partir desses dados. Foi utilizada comunicação I2C para integração com o display LCD 16x2 e com os sensores, além de controle remoto infravermelho usando o receptor VS1838B para operação manual. A lógica de programação incluiu diferentes algoritmos para cada modo de operação, garantindo que o robô pudesse alternar entre seguir linha, estimar atrito e responder a comandos remotos.

## Desafios e Erros ⚠️
Durante o desenvolvimento, houve dificuldades na sincronização e precisão das leituras dos sensores infravermelhos, que afetaram a tomada de decisão do robô e, em alguns casos, impediram seu movimento. A integração entre hardware e software exigiu ajustes finos e reformulação da lógica, pois cada modo de operação demandava algoritmos distintos. O cálculo do coeficiente de atrito apresentou margem de erro devido à imprecisão do acelerômetro, resistência dos motores, falta de sincronismo entre os quatro motores, resistência do ar e ruídos aleatórios. Na parte elétrica, a substituição da fonte de 9V por uma de 12V aumentou o desempenho, mas também o risco de sobrecarga, exigindo cuidados extras para evitar danos aos componentes. A organização dos fios também foi um ponto crítico, já que a desorganização aumentava o risco de curtos e dificultava a manutenção.
﻿
