import json

def convert_mtl_to_json(mtl_file, json_file):
    # Initialize a dictionary to hold the material data
    material_dict = {}

    # Open the .mtl file
    with open(mtl_file, 'r') as f:
        lines = f.readlines()

    # Parse the .mtl file line by line
    for line in lines:
        # Split the line into words
        words = line.split()
        if words:
            # If the line defines a new material, add it to the dictionary
            if words[0] == 'newmtl':
                material_name = words[1]
                material_dict[material_name] = {}

            # If the line defines a property of the current material, add it to the dictionary
            elif words[0] in ['Ns', 'Ka', 'Kd', 'Ks', 'Ke', 'Ni', 'd', 'illum']:
                property_name = words[0]
                property_values = [float(value) for value in words[1:]]
                material_dict[material_name][property_name] = property_values

    # Write the dictionary to a .json file
    with open(json_file, 'w') as f:
        json.dump(material_dict, f)


convert_mtl_to_json('./obj_files/elephant.mtl', './obj_files/elephant.mtl.json')
