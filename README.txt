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

Todos os projetos nesse repositório foram construídos utilizando o compilador 
g++, como mandado pela especificação dos projetos. Porém, todos requerem versão 
maior ou igual a onze desse compilador, devido ao uso extenso de funcionalidades
do C++20, especialmente módulos. Os projetos foram desenvolvidos no Arch Linux, 
rodando em WSL 2.

##############
# Trabalho 1 #
##############

O primeiro trabalho se encontra sob o diretório "Socket/" e tanto o cliente 
quanto o servidor podem ser construídos usando o target "Socket" do makefile
raíz. Após sua construção, esse projeto produzirá dois binários, um servidor em
"Socket/Server" e, analogamente, um cliente em "Socket/Cliente".
