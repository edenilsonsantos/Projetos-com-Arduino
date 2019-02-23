# Introdução e Objetivos 

Introdução:
A ideia do projeto é resolver alguns problemas de segurança como temperatura irregular para o ambiente, e foco de incêndio, utilizando soluções de baixo custo.

Objetivo:
Monitorar o ambiente com sensores, e no caso de identificado: temperatura irregular, fogo ou fumaça, automaticamente será enviando um SMS para o responsável da área e uma medida de emergência será acionada no local.

# Metodologia

Se identificado temperatura irregular, envia SMS, e como medida de emergência no local liga um relay, que aciona um ar condicionado de backup, exaustores ou ventiladores.
Se identificado fogo ou fumaça, envia um SMS, e como medida de emergência no local, liga um relay, que aciona o disparo de um cilindro de CO2.

Utilizado hardware de baixo custo:
Arduino mega, sensor de fumaça, sensor de fogo, sensor de temperatura, display oled, placa de rede, relays, e mini cilindro de CO2 de 16g (pressão de 40libras).

Os SMS são enviados pela rede, sem necessidade de chip de operadora, é utilizado no código do Arduino um API de serviço de SMS na nuvem.
Também é possível utilizar o protocolo SNMP no Arduino, caso a empresa já possua algum software de monitoramento, esta solução pode ser integrada.
