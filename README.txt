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


