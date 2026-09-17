struct Pose {
    double px, py, pz;
    double qx, qy, qz, qw;
};

void appliquerPose(
    const Pose& pose,
    double x, double y, double z,
    double& X, double& Y, double& Z
) 

{
    double tx = 2.0 * (pose.qy * z - pose.qz * y);
    double ty = 2.0 * (pose.qz * x - pose.qx * z);
    double tz = 2.0 * (pose.qx * y - pose.qy * x);

    double rx = x + pose.qw * tx + (pose.qy * tz - pose.qz * ty);
    double ry = y + pose.qw * ty + (pose.qz * tx - pose.qx * tz);
    double rz = z + pose.qw * tz + (pose.qx * ty - pose.qy * tx);

    X = rx + pose.px;
    Y = ry + pose.py;
    Z = rz + pose.pz;
}

void appliquerTranslationEsuiteLaRotation(
    const Pose& pose,
    double x, double y, double z,
    double& X, double& Y, double& Z
) 

{
    // Translation 
    double tx = x + pose.px;
    double ty = y + pose.py;
    double tz = z + pose.pz;

    // Rotation 
    double a = 2.0 * (pose.qy * tz - pose.qz * ty);
    double b = 2.0 * (pose.qz * tx - pose.qx * tz);
    double c = 2.0 * (pose.qx * ty - pose.qy * tx);

    X = tx + pose.qw * a + (pose.qy * c - pose.qz * b);
    Y = ty + pose.qw * b + (pose.qz * a - pose.qx * c);
    Z = tz + pose.qw * c + (pose.qx * b - pose.qy * a);
}

int main() {
    Pose pose;

    cin >> pose.px >> pose.py >> pose.pz;
    cin >> pose.qx >> pose.qy >> pose.qz >> pose.qw;

    double x, y, z;
    cin >> x >> y >> z;

    double X1, Y1, Z1;
    double X2, Y2, Z2;

    appliquerPose(pose, x, y, z, X1, Y1, Z1);
    appliquerTranslationEnsuiteLaRotation(pose, x, y, z, X2, Y2, Z2);

    cout << fixed << setprecision(4);

    cout << "Rotation puis translation :" << endl;
    cout << X1 << " " << Y1 << " " << Z1 << endl;

    cout << "Translation puis rotation :" << endl;
    cout << X2 << " " << Y2 << " " << Z2 << endl;

    return 0;
}

par exemple :

Pose :
0 0 0
0 0 0 1

Point :
4 5 6

Les deux opérations donnent :

Rotation puis translation :
4.0000 5.0000 6.0000

Translation puis rotation :
4.0000 5.0000 6.0000
Parce que la translation est nulle : ajouter (0,0,0) ne modifie pas le point. 
Les deux méthodes reviennent donc simplement à appliquer la même rotation au point.

On peut aussi obtenir une coïncidence avec une translation non nulle si la translation est parallèle à l'axe de rotation.
