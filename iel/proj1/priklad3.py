#Roland Schulz
#2019-12-19
#Calculation of the third task of first IEL course project
#
#run instructions:
#   python priklad3.py <"A" - "H">


import numpy
import sys
zadani_skupiny = {
    "A" :{"U":120, "I1": 0.9, "I2": 0.7, "R1":53, "R2":49, "R3":65, "R4":39, "R5":32},
    "B" :{"U":150, "I1": 0.7, "I2": 0.8, "R1":49, "R2":45, "R3":61, "R4":34, "R5":34}, 
    "C" :{"U":110, "I1":0.85, "I2":0.75, "R1":44, "R2":31, "R3":56, "R4":20, "R5":30},
    "D" :{"U":115, "I1": 0.6, "I2": 0.9, "R1":50, "R2":38, "R3":48, "R4":37, "R5":28},
    "E" :{"U":135, "I1":0.55, "I2":0.65, "R1":52, "R2":42, "R3":52, "R4":42, "R5":21},
    "F" :{"U":145, "I1":0.75, "I2":0.85, "R1":48, "R2":44, "R3":53, "R4":36, "R5":25},
    "G" :{"U":160, "I1":0.65, "I2":0.45, "R1":46, "R2":41, "R3":53, "R4":33, "R5":29},
    "H" :{"U":130, "I1":0.95, "I2":0.50, "R1":47, "R2":39, "R3":58, "R4":28, "R5":25},
    }

# zjisteni zadani patriciho ke skupine
if len(sys.argv) == 2:
    moje_skupina = sys.argv[1]
else:
    print("Zadejte jeden parametr, vasi skupinu (A-H)")
    quit()

moje_zadani = zadani_skupiny[moje_skupina]

def pprint_arr(arr):
    for row in arr:
        print(row)
    print()

print("Moje zadani:")
print( moje_skupina, ": ", moje_zadani)

#zpracovani vstupu
U  = moje_zadani["U"]
I1 = moje_zadani["I1"]
I2 = moje_zadani["I2"]
R1 = moje_zadani["R1"]
R2 = moje_zadani["R2"]
R3 = moje_zadani["R3"]
R4 = moje_zadani["R4"]
R5 = moje_zadani["R5"]

#vypocet vodivosti
print("Vypocet vodivosti:")
print("obecny vzorec: Gx = 1/Rx")
G1 = 1/R1
print("G1 = ", G1, "[S]")
G2 = 1/R2
print("G2 = ", G2, "[S]")
G3 = 1/R3
print("G3 = ", G3, "[S]")
G4 = 1/R4
print("G4 = ", G4, "[S]")
G5 = 1/R5
print("G5 = ", G5, "[S]")
print()

# vytvoreni matice na vyreseni soustavy rovnic Cramerem
a = -(G1+G2)
b = G2
c = 0
d = G2
e = -(G2+G4+G5)
f = G4+G5
g = 0
h = G4+G5
i = -(G4+G3+G5)

leva1 = [a, b, c]
leva2 = [d, e, f]
leva3 = [g, h, i]
leva = [leva1, leva2, leva3]

print("Vytvoreni matic na Cramera:")
print("leva strana matice:")
pprint_arr(leva)

prava1 = -I1
prava2 = -G5*U
prava3 = (G5*U) - I2
prava = [prava1, prava2, prava3]
print("prava strana matice: ")
pprint_arr(prava)

# vytvoreni matic na Cramera
A  = numpy.array([leva1, leva2, leva3])
A1 = numpy.array([prava, leva2, leva3])
A2 = numpy.array([leva1, prava, leva3])
A3 = numpy.array([leva1, leva2, prava])

print("Sefovska matice A:")
pprint_arr(A)
print("submatice A1:")
pprint_arr(A1)
print("submatice A2:")
pprint_arr(A2)
print("submatice A3:")
pprint_arr(A3)

# vypocet determinantu matic
detA = numpy.linalg.det(A)
detA1 = numpy.linalg.det(A1)
detA2 = numpy.linalg.det(A2)
detA3 = numpy.linalg.det(A3)

print("det. A=", detA)
print("det. A1=", detA1)
print("det. A2=", detA2)
print("det. A3=", detA3)

# aplikovani Cramera
UA = detA1/detA
UB = detA2/detA
UC = detA3/detA

print("aplikovani Cramera:")
print("UA = detA1/detA =", UA, "[V]")
print("UB = detA2/detA =", UB, "[V]")
print("UC = detA3/detA =", UC, "[V]")

IR1 = (UA)/R1
IR2 = (UA-UB)/R2
IR3 = (UC)/R3
IR4 = (UB-UC)/R4
IR5 = (U+UC-UB)/R5

UR4 = UB - UC

print("Vysledek:")
print("IR4 =", IR4 ," [A]")
print("UR4 =", UR4 ," [V]")
