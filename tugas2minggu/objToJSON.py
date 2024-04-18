import json

def obj_to_json(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    vertices = []
    faces = []

    for line in lines:
        if line.startswith('v '):
            vertex = line.split()[1:]
            vertices.append(vertex)
        elif line.startswith('f '):
            face = line.split()[1:]
            faces.append(face)

    obj_data = {
        'vertices': vertices,
        'faces': faces
    }

    json_data = json.dumps(obj_data)
    return json_data

# Usage example
file_path = './obj_files/Low Poly Elephant-blendswap.obj'
json_data = obj_to_json(file_path)
json_file_path = file_path.replace('.obj', '.json')
with open(json_file_path, 'w') as json_file:
    json_file.write(json_data)
# print(json_data)