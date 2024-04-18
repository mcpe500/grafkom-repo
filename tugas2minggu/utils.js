function subtract(a, b) {
  return a.map((val, i) => val - b[i]);
}

function cross(a, b) {
  return [
    a[1] * b[2] - a[2] * b[1],
    a[2] * b[0] - a[0] * b[2],
    a[0] * b[1] - a[1] * b[0],
  ];
}

function normalize(a) {
  const length = Math.sqrt(a.reduce((acc, val) => acc + val * val, 0));
  return a.map((val) => val / length);
}

function dot(a, b) {
  return a.reduce((acc, val, i) => acc + val * b[i], 0);
}

function calculateNormal(face, vertices) {
  // Mengambil tiga titik pertama dari wajah untuk membentuk dua vektor
  let point1 = vertices[face[0]];
  let point2 = vertices[face[1]];
  let point3 = vertices[face[2]];
  // Menghitung dua vektor yang membentuk bidang wajah
  let vector1 = subtract(point2, point1);
  let vector2 = subtract(point3, point1);

  // Menghitung normal dengan mengambil produk silang dari dua vektor ini
  let normal = cross(vector1, vector2);

  // Normalisasi normal sehingga panjangnya adalah 1
  normal = normalize(normal);

  return normal;
}

const generateIdentityMatrix = () => {
  return [
    [1, 0, 0, 0],
    [0, 1, 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1],
  ];
};

const generateTanslasiMatrix = (dx, dy, dz) => {
  const T = [
    [1, 0, 0, dx],
    [0, 1, 0, dy],
    [0, 0, 1, dz],
    [0, 0, 0, 1],
  ];
  return T;
};
const generateInverseTanslasiMatrix = (dx, dy, dz) => {
  const T = [
    [1, 0, 0, -dx],
    [0, 1, 0, -dy],
    [0, 0, 1, -dz],
    [0, 0, 0, 1],
  ];
  return T;
};
const generateRotasiMatrix = (derajat) => {
  const radian = (derajat * Math.PI) / 180;
  const R = [
    [Math.cos(radian), -Math.sin(radian), 0, 0],
    [Math.sin(radian), Math.cos(radian), 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1],
  ];
  return R;
};
const generateInverseRotasiMatrix = (derajat) => {
  const radian = (derajat * Math.PI) / 180;
  const R = [
    [Math.cos(radian), Math.sin(radian), 0, 0],
    [-Math.sin(radian), Math.cos(radian), 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1],
  ];
  return R;
};
const generateScalingMatrix = (sx, sy, sz) => {
  const S = [
    [sx, 0, 0, 0],
    [0, sy, 0, 0],
    [0, 0, sz, 0],
    [0, 0, 0, 1],
  ];
  return S;
};
const generateInverseScalingMatrix = (sx, sy, sz) => {
  const S = [
    [1 / sx, 0, 0, 0],
    [0, 1 / sy, 0, 0],
    [0, 0, 1 / sz, 0],
    [0, 0, 0, 1],
  ];
  return S;
};
function multiplyMatrixAndPoint(matrix, point) {
  // Convert the point to homogeneous coordinates
  const homogeneousPoint = [point.x, point.y, point.z, 1];

  let result = [0, 0, 0, 0];

  for (let i = 0; i < 4; ++i) {
    for (let j = 0; j < 4; ++j) {
      result[i] += matrix[i][j] * homogeneousPoint[j];
    }
  }

  // Convert the result back to 3D coordinates
  return { x: result[0], y: result[1], z: result[2] };
}
