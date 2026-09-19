struct Pose {
    double px, py, pz;
    double qx, qy, qz, qw;
};

struct Vecteur {
    double x, y, z;
};

// ============================================================
// Normalisation d'un quaternion
// ============================================================
void normaliserQuaternion(Pose& p)
{
    double n = sqrt(
        p.qx*p.qx +
        p.qy*p.qy +
        p.qz*p.qz +
        p.qw*p.qw
    );

    if (n > 1e-12) {
        p.qx /= n;
        p.qy /= n;
        p.qz /= n;
        p.qw /= n;
    }
}

// ============================================================
// Multiplication de deux quaternions
// ============================================================
void multiplierQuaternion(
    double ax, double ay, double az, double aw,
    double bx, double by, double bz, double bw,
    double& x, double& y, double& z, double& w)
{
    x = aw*bx + ax*bw + ay*bz - az*by;
    y = aw*by - ax*bz + ay*bw + az*bx;
    z = aw*bz + ax*by - ay*bx + az*bw;
    w = aw*bw - ax*bx - ay*by - az*bz;
}

// ============================================================
// Avance une pose de dt secondes
// avec vitesse linéaire et vitesse angulaire constantes
// ============================================================
Pose avancerPose(
    const Pose& pose,
    const Vecteur& vitesseLineaire,
    const Vecteur& vitesseAngulaire,
    double dt)
{
    Pose resultat = pose;

    // --------------------------------------------------------
    // 1. Translation
    // --------------------------------------------------------
    resultat.px += vitesseLineaire.x * dt;
    resultat.py += vitesseLineaire.y * dt;
    resultat.pz += vitesseLineaire.z * dt;

    // --------------------------------------------------------
    // 2. Vitesse angulaire
    // --------------------------------------------------------
    double normeOmega = sqrt(
        vitesseAngulaire.x * vitesseAngulaire.x +
        vitesseAngulaire.y * vitesseAngulaire.y +
        vitesseAngulaire.z * vitesseAngulaire.z
    );

    // --------------------------------------------------------
    // Cas particulier : vitesse angulaire nulle
    // --------------------------------------------------------
    if (normeOmega < 1e-12) {
        return resultat;
    }

    // --------------------------------------------------------
    // 3. Angle parcouru
    // --------------------------------------------------------
    double angle = normeOmega * dt;

    // Axe de rotation unitaire
    double ax = vitesseAngulaire.x / normeOmega;
    double ay = vitesseAngulaire.y / normeOmega;
    double az = vitesseAngulaire.z / normeOmega;

    // --------------------------------------------------------
    // 4. Quaternion correspondant à cette rotation
    // --------------------------------------------------------
    double demiAngle = angle / 2.0;

    double s = sin(demiAngle);
    double c = cos(demiAngle);

    double rx = ax * s;
    double ry = ay * s;
    double rz = az * s;
    double rw = c;

    // --------------------------------------------------------
    // 5. Nouvelle orientation
    // --------------------------------------------------------
    double qx, qy, qz, qw;

    multiplierQuaternion(
        pose.qx, pose.qy, pose.qz, pose.qw,
        rx, ry, rz, rw,
        qx, qy, qz, qw
    );

    resultat.qx = qx;
    resultat.qy = qy;
    resultat.qz = qz;
    resultat.qw = qw;

    normaliserQuaternion(resultat);

    return resultat;
}

// ============================================================
// Programme principal
// ============================================================
int main()
{
    Pose pose;

    Vecteur vitesseLineaire;
    Vecteur vitesseAngulaire;

    double dt;

    // Pose initiale
    cout << "Pose initiale (px py pz qx qy qz qw) : ";
    cin >> pose.px
        >> pose.py
        >> pose.pz
        >> pose.qx
        >> pose.qy
        >> pose.qz
        >> pose.qw;

    // Vitesse lineaire
    cout << "Vitesse lineaire (vx vy vz) : ";
    cin >> vitesseLineaire.x
        >> vitesseLineaire.y
        >> vitesseLineaire.z;

    // Vitesse angulaire
    cout << "Vitesse angulaire (wx wy wz) : ";
    cin >> vitesseAngulaire.x
        >> vitesseAngulaire.y
        >> vitesseAngulaire.z;

    // Duree
    cout << "Duree dt : ";
    cin >> dt;

    Pose resultat = avancerPose(
        pose,
        vitesseLineaire,
        vitesseAngulaire,
        dt
    );

    cout << fixed << setprecision(6);

    cout << "\nPose extrapolee :\n";

    cout << "Position : "
         << resultat.px << " "
         << resultat.py << " "
         << resultat.pz << endl;

    cout << "Quaternion : "
         << resultat.qx << " "
         << resultat.qy << " "
         << resultat.qz << " "
         << resultat.qw << endl;

    return 0;
}
