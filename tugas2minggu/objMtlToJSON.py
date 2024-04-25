import json


def convert_obj_mtl_to_json(obj_path,output_path):
    # Membaca file .mtl dan .obj
    with open(obj_path, 'r') as obj_file:
        obj_data = obj_file.readlines()

    # Membuat dictionary untuk menyimpan data
    data = {
        "vertices": [],
        "faces": []
    }

    # Mengisi data vertices
    for line in obj_data:
        if line.startswith('v '):
            _, x, y, z = line.split()
            data["vertices"].append([x, y, z])

    # Mengisi data faces
    current_material = None
    for line in obj_data:
        if line.startswith('usemtl'):
            _, current_material = line.split()
        elif line.startswith('f'):
            face_data = line.split()[1:]
            face_data.append(current_material)
            data["faces"].append(face_data)

    # Menulis data ke file .json
    with open(output_path, 'w') as json_file:
        json.dump(data, json_file, indent=4)


convert_obj_mtl_to_json("./obj_files/Godzilla/FabConvert.com_godzilla.obj","./obj_files/godzilla.obj.json")