#############################################
## Repositório de projetos do Meneguette   ##
#############################################
## Alunos:                                 ##
##     Matheus Branco Borella  (11218897)  ##
##     Natan ***REMOVED*** (***REMOVED***) ##
#############################################

Esse repositório contém todos os projetos desenvolvidos pelo nosso grupo durante
o semestre na matéria de Redes de Computadores (SSC-0142), com seus respectivos
Makefiles.

Todos os projetos nesse repositório foram construídos utilizando os compiladores
clang (versão 13.0.0) e gcc (versão 11.1.0). O código foi desenvolvido em C17, em
Arch Linux e Ubuntu 20.04, rodando em WSL 2.

##############
# Trabalho 1 #
##############

O primeiro trabalho se encontra sob o diretório "Messaging/" e tanto o cliente 
quanto o servidor podem ser construídos usando o target "all" do makefile.
Após sua construção, esse projeto produzirá dois binários, "server" e
"client".

O servidor deve ser iniciado primeiro e não aceita nenhum argumento. Ele espera
por conexões no endereço 0.0.0.0 e porta 3621

Os clientes precisam receber o nome como argumento. Cada cliente conecta com
um servidor no endereço 127.0.0.1 e porta 3621, lê linhas da sua entrada padrão
e às envia ao servidor, que retransmite para todos os clientes.

##############
# Trabalho 2 #
##############

O segundo trabalho se encontra sob o diretório "Layers/" e pode ser
construído com o target "all" do makefile.
Após sua construção, esse projeto produzirá um binário, "layers".

O programa utiliza duas variáveis de ambiente para configuração,
ERROR_RATE (valor, em %, de 0 a 100 de bits que serão invertidos) e
ERROR_CHECK_MODE (modo para verificação de erro, podendo ser `parity_odd`,
`parity_even` ou `crc32`).

Ao ser iniciado, o programa lê mensagens da entrada padrão e simula o envio
dela entre dois computadores, escrevendo na saída padrão os bits em cada
computador, o resultado da verificação de erro e a mensagem recebida.

Os bits recebidos possuem uma cor vermelha caso sejam diferentes dos bits enviados,
para facilitar a visualização dos erros na transmissão.

Caracteres não printáveis recebidos pelo segundo computador também possuem
cor vermelha, e são representados em hexadecimal (`0xYY`).
