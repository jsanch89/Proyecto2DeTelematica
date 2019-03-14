# Algoritmo de Vector de Movimiento

```
Jenny Quiroz Muñoz - jquirozm@eafit.edu.co
Julian Andres Sanchez Alzate - jsanch89@eafit.edu.co
Anderson Daniel Grajales Alzate - agrajal7@eafit.edu.co
```

## Definición del problema

El algoritmo de compresión de video MPEG, utiliza una técnica conocida como Vectores de Movimiento, el cual le permite realizar la compresión temporal de frames P y B. [https://es.wikipedia.org/wiki/MPEG-2](https://es.wikipedia.org/wiki/MPEG-2). \
La técnica consiste en predecir hacia donde se mueven los macro-bloques (16 x 16 pixeles) de un frame i en el frame i+1. Se realiza una búsqueda hacia delante (frames P) y además hacía atrás (frames B). Para este ejercicio solo se realizará búsqueda hacia delante. \
![Macrobloque1](macrobloque1.jpg)
Cada macro-bloque se debe buscar en el frame siguiente en una región de búsqueda. Existen múltiples heurísticas para determinar la región de búsqueda. Para efectos de este problema, se tomará toda el frame i+1 como la región de búsqueda. \
![Macrobloque1](macrobloque2.jpg)
A continuación se presenta la función de similitud entre dos (2) macro-bloques: \
<a href="https://www.codecogs.com/eqnedit.php?latex=\sum\limits_{i&space;=&space;0}^{15}\sum\limits_{j&space;=&space;0}^{15}{\mid&space;\mathcal{V}_n(x&space;&plus;&space;i,&space;y&space;&plus;&space;j)&space;-&space;\mathcal{V}_m(x&plus;dx&plus;i,&space;y&space;&plus;&space;dy&space;&plus;&space;j)\mid}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\sum\limits_{i&space;=&space;0}^{15}\sum\limits_{j&space;=&space;0}^{15}{\mid&space;\mathcal{V}_n(x&space;&plus;&space;i,&space;y&space;&plus;&space;j)&space;-&space;\mathcal{V}_m(x&plus;dx&plus;i,&space;y&space;&plus;&space;dy&space;&plus;&space;j)\mid}" title="\sum\limits_{i = 0}^{15}\sum\limits_{j = 0}^{15}{\mid \mathcal{V}_n(x + i, y + j) - \mathcal{V}_m(x+dx+i, y + dy + j)\mid}" /></a> \
Donde Vn  es el macro-bloque de referencia (en el frame i), Vm es el macro-bloque destino (en el frame i+1). dx y dy representan los desplazamiento en la región de búsqueda, por ejemplo, si dx=0 y dy=0, se está intentando hacer matching exactamente en la misma posición. \
El criterio para moverse dentro de la región de búsqueda, no es parte del estándar, para el caso de este ejercicio se utilizará búsqueda exhaustiva, es decir, en el peor de los casos (si no lo encuentra), se recorre toda la región de búsqueda con incrementos en dx y dy de un (1) pixel.
# Módulos Usados:
```
  Numpy
  Pillow
```

# Serial (Ejecución)
Para ejecutar esta versión del algoritmo es necesario tener python 3.6.0 y los módulos anteriormente mencionados. Luego de asegurarse que están instalados, debe abrir la terminal o línea de comandos. Finalmente, ingrese a la carpeta serial y ejecute el comando: 

```
  python VectorDeMovimiento.py
```

