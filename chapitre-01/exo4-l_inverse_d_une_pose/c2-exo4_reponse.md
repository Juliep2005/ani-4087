struct Pose {
    double px, py, pz;
    double qx, qy, qz, qw;
};

// Rotation d'un point par un quaternion
void tourner(
    double qx, double qy, double qz, double qw,
    double x, double y, double z,
    double& rx, double& ry, double& rz
) {
    double tx = 2.0 * (qy * z - qz * y);
    double ty = 2.0 * (qz * x - qx * z);
    double tz = 2.0 * (qx * y - qy * x);

    rx = x + qw * tx + (qy * tz - qz * ty);
    ry = y + qw * ty + (qz * tx - qx * tz);
    rz = z + qw * tz + (qx * ty - qy * tx);
}

// Applique une pose : rotation puis translation
void appliquer(
    const Pose& pose,
    double x, double y, double z,
    double& X, double& Y, double& Z
) {
    double rx, ry, rz;

    tourner(
        pose.qx, pose.qy, pose.qz, pose.qw,
        x, y, z,
        rx, ry, rz
    );

    X = rx + pose.px;
    Y = ry + pose.py;
    Z = rz + pose.pz;
}

// Calcule l'inverse d'une pose
Pose Inverser(const Pose& pose) {
    Pose inverse;

    // Conjugué du quaternion
    inverse.qx = -pose.qx;
    inverse.qy = -pose.qy;
    inverse.qz = -pose.qz;
    inverse.qw =  pose.qw;

    // Position opposée tournée par le quaternion conjugué
    double x = -pose.px;
    double y = -pose.py;
    double z = -pose.pz;

    tourner(
        inverse.qx, inverse.qy, inverse.qz, inverse.qw,
        x, y, z,
        inverse.px, inverse.py, inverse.pz
    );

    return inverse;
}

int main() {
    Pose pose;

    // Lecture de la pose
    cin >> pose.px >> pose.py >> pose.pz;
    cin >> pose.qx >> pose.qy >> pose.qz >> pose.qw;

    // Lecture du point
    double x, y, z;
    cin >> x >> y >> z;

    // Application de la pose
    double x1, y1, z1;
    appliquer(pose, x, y, z, x1, y1, z1);

    // Calcul de l'inverse
    Pose inverse = Inverser(pose);

    // Application de l'inverse au résultat
    double x2, y2, z2;
    appliquer(inverse, x1, y1, z1, x2, y2, z2);

    // Calcul de l'écart avec le point initial
    double dx = x2 - x;
    double dy = y2 - y;
    double dz = z2 - z;

    cout << fixed << setprecision(6);

    cout << "Point apres la pose : "
         << x1 << " " << y1 << " " << z1 << endl;

    cout << "Point apres la pose inverse : "
         << x2 << " " << y2 << " " << z2 << endl;

    cout << "Ecart : "
         << dx << " " << dy << " " << dz << endl;

    return 0;
}
