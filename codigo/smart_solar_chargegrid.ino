const int sensorSolar = A0;
const int botaoVeiculo = 7;

const int ledVermelho = 2;
const int ledAmarelo = 5;
const int ledVerde = 12;

const float potenciaCarregadorKW = 7.4;
const float fatorCO2 = 0.35;

const float tarifaBaixaSolar = 1.30;
const float tarifaMediaSolar = 1.00;
const float tarifaAltaSolar = 0.80;

unsigned long inicioSessao = 0;
unsigned long ultimoCalculo = 0;

float minutosSimulados = 0;
float energiaConsumidaKWh = 0;
float valorTotal = 0;
float co2Evitado = 0;
float somaPercentualSolar = 0;
int quantidadeLeituras = 0;

bool sessaoAtiva = false;
bool veiculoAnteriorConectado = false;

void setup()
{
  pinMode(botaoVeiculo, INPUT_PULLUP);

  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  Serial.begin(9600);

  Serial.println("====================================");
  Serial.println("SMART SOLAR CHARGEGRID");
  Serial.println("Sprint 2 - Prova de Conceito");
  Serial.println("====================================");
  Serial.println("Sistema iniciado.");
  Serial.println("Aguardando veiculo...");
  Serial.println();
}

void loop()
{
  int leituraSolar = analogRead(sensorSolar);

  bool veiculoConectado = (digitalRead(botaoVeiculo) == LOW);

  atualizarLeds(leituraSolar);

  if (veiculoConectado && !veiculoAnteriorConectado)
  {
    iniciarSessao();
  }

  if (veiculoConectado && sessaoAtiva)
  {
    calcularSessao(leituraSolar);
  }

  if (!veiculoConectado && veiculoAnteriorConectado)
  {
    encerrarSessao();
  }

  veiculoAnteriorConectado = veiculoConectado;

  delay(100);
}

void atualizarLeds(int leituraSolar)
{
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVerde, LOW);

  if (leituraSolar < 341)
  {
    digitalWrite(ledVermelho, HIGH);
  }
  else if (leituraSolar < 682)
  {
    digitalWrite(ledAmarelo, HIGH);
  }
  else
  {
    digitalWrite(ledVerde, HIGH);
  }
}

void iniciarSessao()
{
  sessaoAtiva = true;

  inicioSessao = millis();
  ultimoCalculo = millis();

  minutosSimulados = 0;
  energiaConsumidaKWh = 0;
  valorTotal = 0;
  co2Evitado = 0;
  somaPercentualSolar = 0;
  quantidadeLeituras = 0;

  Serial.println("====================================");
  Serial.println("VEICULO CONECTADO");
  Serial.println("Sessao de recarga iniciada.");
  Serial.println("Cada 1 segundo real representa 1 minuto simulado.");
  Serial.println("====================================");
  Serial.println();
}

void calcularSessao(int leituraSolar)
{
  unsigned long agora = millis();

  if (agora - ultimoCalculo >= 1000)
  {
    ultimoCalculo = agora;

    minutosSimulados += 1;

    float percentualSolar = map(leituraSolar, 0, 1023, 0, 100);
    float tarifaAtual = calcularTarifa(leituraSolar);

    float energiaNoMinuto = potenciaCarregadorKW / 60.0;

    energiaConsumidaKWh += energiaNoMinuto;
    valorTotal += energiaNoMinuto * tarifaAtual;

    float energiaSolarUsada = energiaNoMinuto * (percentualSolar / 100.0);
    co2Evitado += energiaSolarUsada * fatorCO2;

    somaPercentualSolar += percentualSolar;
    quantidadeLeituras++;

    Serial.print("Tempo: ");
    Serial.print(minutosSimulados, 0);
    Serial.print(" min | Solar: ");
    Serial.print(percentualSolar, 0);
    Serial.print("% | Consumo: ");
    Serial.print(energiaConsumidaKWh, 2);
    Serial.print(" kWh | Tarifa: R$ ");
    Serial.print(tarifaAtual, 2);
    Serial.print("/kWh | Total: R$ ");
    Serial.println(valorTotal, 2);
  }
}

float calcularTarifa(int leituraSolar)
{
  if (leituraSolar < 341)
  {
    return tarifaBaixaSolar;
  }
  else if (leituraSolar < 682)
  {
    return tarifaMediaSolar;
  }
  else
  {
    return tarifaAltaSolar;
  }
}

void encerrarSessao()
{
  sessaoAtiva = false;

  float mediaSolar = 0;

  if (quantidadeLeituras > 0)
  {
    mediaSolar = somaPercentualSolar / quantidadeLeituras;
  }

  Serial.println();
  Serial.println("====================================");
  Serial.println("VEICULO DESCONECTADO");
  Serial.println("Sessao de recarga encerrada.");
  Serial.println("------------------------------------");

  Serial.print("Tempo total: ");
  Serial.print(minutosSimulados, 0);
  Serial.println(" minutos simulados");

  Serial.print("Energia consumida: ");
  Serial.print(energiaConsumidaKWh, 2);
  Serial.println(" kWh");

  Serial.print("Media de energia solar: ");
  Serial.print(mediaSolar, 0);
  Serial.println("%");

  Serial.print("Valor total: R$ ");
  Serial.println(valorTotal, 2);

  Serial.print("CO2 evitado: ");
  Serial.print(co2Evitado, 2);
  Serial.println(" kg");

  Serial.println("------------------------------------");

  if (mediaSolar >= 67)
  {
    Serial.println("Classificacao: Recarga sustentavel");
    Serial.println("Alta utilizacao de energia solar.");
  }
  else if (mediaSolar >= 34)
  {
    Serial.println("Classificacao: Recarga mista");
    Serial.println("Uso parcial de energia solar.");
  }
  else
  {
    Serial.println("Classificacao: Baixa geracao solar");
    Serial.println("Maior dependencia da rede eletrica.");
  }

  Serial.println("====================================");
  Serial.println();
  Serial.println("Aguardando novo veiculo...");
  Serial.println();
}