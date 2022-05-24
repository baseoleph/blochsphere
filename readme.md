# blochsphere
<p align="center">
  <img src="https://user-images.githubusercontent.com/63150311/169586700-1a92ff1f-a8c9-4953-9b4a-48d865e116c7.png" alt="blochsphere"/>
 </p>


**blochsphere** a Bloch sphere emulator program.

The program features:

- Adjustment of the number of active spheres (qubits) from 1 to 5;
- Setting a qubit using any of the three
  interpretations: $(\theta, \phi), (\alpha, \beta), (x, y, z)$; note that $\alpha$ is real number;
- Generating a random qubit;
- Using one of [decompositions](https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix) to generate operator.
- Quickly applying standard operators;
- Applying operators to a single specific sphere or to all spheres at once;
- Defining an operator using rotation vector and its angle;
- Adjustment of the operator application speed;

![blochsphere](https://user-images.githubusercontent.com/63150311/169410078-d3182e31-3d35-48f4-b66f-38f079ff4457.png)

**blochshere** is based on [BLOCH](https://github.com/baseoleph/blochsphere/tree/base). The architecture has been
redesigned, the performance and the reliability of the algorithms have been improved. Operator decomposition algorithms
in composition of rotations are kindly provided by M.V. Shvetskiy. Logo designed by N. Tomsha (nastya.to2010@yandex.ru).

The program is written using Qt5 (and Qt4 for Windows XP support) and OpenGL.