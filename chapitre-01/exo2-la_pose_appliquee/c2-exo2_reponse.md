struct Pose {
    double px, py, pz;  // position
    double qx, qy, qz, qw;  // quaternion
};

void rotationQuaternion(
    double qx, double qy, double qz, double qw,
    double x, double y, double z,
    double &rx, double &ry, double &rz
) 
{
    double tx = 2.0 * (qy * z - qz * y);
    double ty = 2.0 * (qz * x - qx * z);
    double tz = 2.0 * (qx * y - qy * x);

    rx = x + qw * tx + (qy * tz - qz * ty);
    ry = y + qw * ty + (qz * tx - qx * tz);
    rz = z + qw * tz + (qx * ty - qy * tx);
}

int main() {
    Pose pose;

    // Lecture de la position et du quaternion
    cin >> pose.px >> pose.py >> pose.pz;
    cin >> pose.qx >> pose.qy >> pose.qz >> pose.qw;

    // Lecture du point
    double x, y, z;
    cin >> x >> y >> z;

    // Rotation
    double rx, ry, rz;
    rotationQuaternion(
        pose.qx, pose.qy, pose.qz, pose.qw,
        x, y, z,
        rx, ry, rz
    );

    // Translation
    double X = rx + pose.px;
    double Y = ry + pose.py;
    double Z = rz + pose.pz;

    // Affichage
    cout << fixed << setprecision(4);
    cout << X << endl;
    cout << Y << endl;
    cout << Z << endl;

    return 0;
}

px py pz
