import json

def obj_to_json(obj_filename, json_filename):
    vertices = []
    faces = []

    with open(obj_filename, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 0:
                continue
            if parts[0] == 'v':
                vertices.append([float(parts[i]) for i in range(1, 4)])
            elif parts[0] == 'f':
                faces.append([int(parts[i].split('/')[0]) for i in range(1, len(parts))])

    data = {'vertices': vertices, 'faces': faces}

    with open(json_filename, 'w') as f:
        json.dump(data, f, indent=4)

# Usage
obj_to_json('kera.obj', 'output.json')
