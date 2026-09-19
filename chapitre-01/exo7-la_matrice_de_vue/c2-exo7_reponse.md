Les deux méthodes donnent les mêmes seize coefficients pour une pose valide, à l'erreur numérique près. La première 
utilise une inversion générale de la matrice, tandis que la seconde exploite directement la structure d'une pose rigide 
en utilisant le conjugué du quaternion et la translation opposée transformée par la rotation inverse. Lorsqu'une pose dégénérée
est fournie à l'inversion générale, celle-ci peut détecter que la matrice n'est pas inversible et signaler un échec. Cela montre l'intérêt de traiter explicitement 
les poses comme des transformations rigides plutôt que d'utiliser systématiquement une inversion matricielle générale.
