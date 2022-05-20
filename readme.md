# blochsphere
<p align="center">
  <img src="https://user-images.githubusercontent.com/63150311/169586700-1a92ff1f-a8c9-4953-9b4a-48d865e116c7.png" alt="blochsphere"/>
 </p>


**blochsphere** a Bloch sphere emulator program.

The program features:

- Adjust the number of active spheres (qubits) from 1 to 5;
- Set a qubit using any of the three
  interpretations: <img src="https://render.githubusercontent.com/render/math?math=(\theta, \phi), (\alpha, \beta), (x, y, z)">
  ; note that <img src="https://render.githubusercontent.com/render/math?math=\alpha"> is real number;
- Generate a random qubit;
- Using one of [decompositions](https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix) to generate operator.
- Quickly apply standard operators;
- Apply operators to a single specific sphere or to all spheres at once;
- Define operator using rotation vector and its angle;
- Adjust the operator application speed;

![blochsphere](https://user-images.githubusercontent.com/63150311/169410078-d3182e31-3d35-48f4-b66f-38f079ff4457.png)

**blochshere** is based on [BLOCH](https://github.com/baseoleph/blochsphere/tree/base). The architecture has been
redesigned, performance has been improved, and reliability of the algorithms has been improved. Algorithms of operator
decomposition in rotation compositions are kindly provided by M.V. Shvetskiy. Logo designed by N. Tomsha (nastya.to2010@yandex.ru).

The program is written using Qt5 (and Qt4 for Windows XP support) and OpenGL.
