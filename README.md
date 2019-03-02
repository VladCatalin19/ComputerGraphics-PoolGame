-------------------------------------------------------------------------------<br />
Vlad Marius-Cătălin 335CC<br />
Anul III Semestrul I<br />
Tema2 Elemente de Grafică pe Calculator<br />
-------------------------------------------------------------------------------<br />
<br />
Pentru această temă am folosit modele 3D pentru masă (1) și pentru tac (2).<br />
Modelul pentru bilă a fost luat din modelul mesei. Camera a fost luată din<br />
laboratorul 5 pentru rotirea în Third Person. Pentru animația tacului, se folo-<br />
sește în vertex shader ca punct de referință timpul de când a fost apăsat buto-<br />
nul care activează animația. Tacul se mișcă sinusoidal pe direcția bilei cu ca-<br />
mera.<br />
Jocul funcționează precum un automat de stări. Are 4 stări, una în care<br />
jucătorul mișcă bila albă pe masă, una pentru rotirea tacului și camerei în<br />
jurul bilei albe și una pentru mișcarea bilelor pe masă. În clasa principală<br />
Level se alege ce stare va urma în funcție de terminarea ultimei stări, astfel<br />
se știe când a fost fault ce stare se va alege, etc.<br />

    Screenshots:
![Demo_1](DEMO_1.png)
![Demo_2](DEMO_2.png)
![Demo_3](DEMO_3.png)

    Referințe:
    (1) https://free3d.com/3d-model/pool-table-v1--600461.html
    (2) https://free3d.com/3d-model/pool-cue-v1--229730.html
