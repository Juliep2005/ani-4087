
struct Pose {
    double px, py, pz;
    double qx, qy, qz, qw;
};

// Rotation d'un point par un quaternion
void tourner(
    const Pose& pose,
    double x, double y, double z,
    double& rx, double& ry, double& rz
) {
    double tx = 2.0 * (pose.qy * z - pose.qz * y);
    double ty = 2.0 * (pose.qz * x - pose.qx * z);
    double tz = 2.0 * (pose.qx * y - pose.qy * x);

    rx = x + pose.qw * tx + pose.qy * tz - pose.qz * ty;
    ry = y + pose.qw * ty + pose.qz * tx - pose.qx * tz;
    rz = z + pose.qw * tz + pose.qx * ty - pose.qy * tx;
}

// Applique une pose : rotation puis translation
void appliquer(
    const Pose& pose,
    double x, double y, double z,
    double& X, double& Y, double& Z
) {
    double rx, ry, rz;

    tourner(pose, x, y, z, rx, ry, rz);

    X = rx + pose.px;
    Y = ry + pose.py;
    Z = rz + pose.pz;
}

// Composition de deux poses : A après B
Pose composer(const Pose& A, const Pose& B) {
    Pose C;

    // Position : A.R(B.p) + A.p
    double rx, ry, rz;
    tourner(A, B.px, B.py, B.pz, rx, ry, rz);

    C.px = rx + A.px;
    C.py = ry + A.py;
    C.pz = rz + A.pz;

    // Quaternion : A.q * B.q
    C.qw = A.qw * B.qw
         - A.qx * B.qx
         - A.qy * B.qy
         - A.qz * B.qz;

    C.qx = A.qw * B.qx
         + A.qx * B.qw
         + A.qy * B.qz
         - A.qz * B.qy;

    C.qy = A.qw * B.qy
         - A.qx * B.qz
         + A.qy * B.qw
         + A.qz * B.qx;

    C.qz = A.qw * B.qz
         + A.qx * B.qy
         - A.qy * B.qx
         + A.qz * B.qw;

    return C;
}

int main() {

    // Première pose
    Pose A = {
        1, 0, 0,
        0, 0, 0, 1
    };

    // Deuxième pose
    Pose B = {
        0, 2, 0,
        0, 0, 0, 1
    };

    // Point de départ
    double x = 3;
    double y = 4;
    double z = 5;

    // 1. Application successive : B puis A
    double xB, yB, zB;
    appliquer(B, x, y, z, xB, yB, zB);

    double x1, y1, z1;
    appliquer(A, xB, yB, zB, x1, y1, z1);

    // 2. Composition A après B
    Pose C = composer(A, B);

    // Application de la pose composée
    double x2, y2, z2;
    appliquer(C, x, y, z, x2, y2, z2);

    // 3. Calcul de l'écart
    double dx = x1 - x2;
    double dy = y1 - y2;
    double dz = z1 - z2;

    cout << fixed << setprecision(6);

    cout << "Application successive : "
         << x1 << " " << y1 << " " << z1 << endl;

    cout << "Pose composee : "
         << x2 << " " << y2 << " " << z2 << endl;

    cout << "Ecart : "
         << dx << " " << dy << " " << dz << endl;

    return 0;
}
