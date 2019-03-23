# Metodología PCAM

## Partición de Datos

### MPI
Se tuvo en cuenta una matrix de (N x M). Para poder resolver el problema se entendió que había oportunidades de paralelismo ya qué cada celda
de la matriz podía computarse de manera independiente. Entonces debido a que se usaban n procesadores para resolver la tarea,
se decidió que cada procesador podía tomar k filas de la matriz. Sin embargo, para que todos los procesadores tengan igual cantidad de datos
o celdas a procesar, se tiene que cumpor que N % k = 0. En el caso donde esto no existen, se asigna a cada procesador a lo sumo una
fila más para procesar. Esta idea se logra haciendo N % k, determinando si es diferente de cero, y en tal caso, a los primeros (n % k) procesadores
se les asigna una fila más.

## Comunicación entre los Procesos
### MPI
Las comunicaciones entre procesos son sincronicas. El Master(Nodo 0) envía a cada esclavo(Nodos 1, 2, ..., n) a lo sumo (n % k + 1) filas 
para que procese. Después, cada esclavo ejecuta el algoritmo para cada una de sus filas. En el momento en que termina, el esclavo envía los datos
al nodo master.


