# Tarea_1_CDR
Conecta 4 en línea

Para comenzar, primero verificar que se tiene instalado el compilador de C++ y Make. Para ello, se puede ejecutar los siguientes comandos en la terminal:

```bash
g++ --version
make --version
```

De no tener instalado alguno de los dos, se puede instalar con los siguientes comandos:

```bash
sudo apt install g++
sudo apt install make
```
Una vez asegurados de que tenemos ambos componentes instalados, nos vamos a la carpeta del proyecto y abrimos una consola dentro del mismo, y construimos ambos ejecutables a través del comando make:

```bash
make
```

Con los archivos "cliente" y "servidor" creados, podemos ejecutarlos en una consola dentro de la misma carpeta.
Para correr el servidor, se ejecuta el siguiente comando:

```bash
./servidor <puerto>
```

Y para el cliente, el siguiente:

```bash
./cliente <ip> <puerto>
```

La dirección IP será 127.0.0.1 para cuando ambos se encuentran en la misma máquina. Recordar que el servidor puede tolerar más de un cliente conectado a este al mismo tiempo.
Ahora sólo toca jugar, buena suerte!