from PIL import Image
import numpy as np

macro_bloque_size = 16
#------------------------------Lectura de imagen como arreglo de rgb falta promediar valores--------------------
"""frame1 = Image.open('../Imagenes/Imagen1.JPG', 'r')
frame1.load()
pix_val_frame1 = np.asarray(frame1, dtype = "int32")
print(len(pix_val_frame1))
#pix_val_frame1 = [(x[0]+x[1]+x[2])/3 for x in frame1.getdata()]

frame2 = Image.open('../Imagenes/Imagen2.JPG', 'r')
frame2.load()
pix_val_frame2 = np.asarray(frame2, dtype = "int32")"""
#----------------------------------------Fin lectura real sin normalizacion--------------------------------------

# -------------------------------------- configuracion de prueba ------------------------------------------------
#Arreglo de pixeles de las imagenes de prueba
pix_val_frame1 = np.random.rand(32,32)

pix_val_frame2 = np.random.rand(32,32)
#---------------------------------------------- fin prueba -----------------------------------------------------

#Tamaño de las imagenes
long_frame1 = len(pix_val_frame1)
long_frame2 = len(pix_val_frame2)

width_frame1 = len(pix_val_frame1[0])
width_frame2 = len(pix_val_frame2[0])

def costCalc(i,j,dx,dy):
    current_cost = 0
    for di in range(macro_bloque_size):
        for dj in range(macro_bloque_size):
             current_cost += abs(pix_val_frame1[i+di][j+dj] - pix_val_frame2[dx+di][dy+dj]) 
    return current_cost

min = 10000
(min_x,min_y)   = (0,0)
(min_dx,min_dy) = (0,0) 
for i in range(width_frame1 - macro_bloque_size+1):
    for j in range(long_frame1 - macro_bloque_size+1 ):
        for dx in range(width_frame2 - macro_bloque_size+1 ):
            for dy in range(long_frame2 - macro_bloque_size+1 ):
                if i!=dx and j!=dy:
                    current_cost = costCalc(i,j,dx,dy)
                    if  current_cost == 0:
                        min = current_cost
                        min_x,min_y = i,j
                        min_dx,min_dy= dx,dy 
                        break
                    else:
                        min = current_cost if current_cost < min else min
                        min_x,min_y = i,j
                        min_dx,min_dy= dx,dy

print("minimo {} en  ({},{}) -> ({},{})".format(min, min_x , min_y , min_dx , min_dy) )