# python-mqtt-client
Cliente desenvolvido para salvar os dados dos sensores no banco de dados.

## Preparando o ambiente

### Criando o ambiente virtual 
Para criar um ambiente virtual, escolha um diretório onde deseja colocá-lo e execute o módulo venv como um script com o caminho do diretório:

``` bash 
python -m venv mqtt-client-env
```

Isso irá criar o diretório tutorial-env se ele não existir e também criará diretórios dentro dele contendo uma cópia do interpretador Python, a biblioteca padrão e diversos arquivos de suporte.

Um diretório de localização comum para um ambiente virtual é .venv. Esse nome tipicamente mantém o diretório oculto em seu ambiente, portanto é transparente, ao menos tempo que explica o motivo desse diretório existir. Também previne conflitos com .env, arquivos de definição de variáveis de ambiente que algumas ferramentas utilizam.

Uma vez criado seu ambiente virtual, você deve ativá-lo.

No Windows, execute:

``` bash 
C:/Users/luiza/luiza-github/venvs/mqtt-client-env/Scripts/activate.bat
```
No Unix ou no MacOS, executa:

``` bash 
source tutorial-env/bin/activate
```
referência: https://docs.python.org/pt-br/3/tutorial/venv.html
