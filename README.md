# images
Un pixel a huit voisins (1 : est, 2 : nord-est, 3 : nord, 4 : nord-ouest, 5 : ouest, 6 : sud-ouest, 7 : sud, 8 : sud-est). Son voisinage est l'ensemble de ces huit voisins plus lui-même.
Modifier l'image pour que chaque pixel prenne la valeur de la moyenne, puis de la moyenne pondérée (un grand poids pour lui-même) de son voisinage.
Modifier l'image pour accentuer le contraste : pour chaque pixel on cherche, dans son voisinage, le pixel dont les couleurs sont les plus éloignées des siennes. On augmente alors d'un tiers ce contraste, éviter les effets de bord (cas du pixel p1 augmenté à cause de son contraste avec p2 et de p2 diminué à cause de son contraste avec le nouveau p1).
On pourra enfin chercher et améliorer les contrastes de "teinte" en mode HSV.
projet proposé par http://www.ai.univ-paris8.fr/~jj/Cours/LI2/PPI2_19.html mon enseingant à paris8
