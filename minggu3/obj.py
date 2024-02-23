import re
import json

def convert_obj_to_js_array(file_path, json_filename):
    vertices = []
  
    with open(file_path, 'r') as obj_file:
        for line in obj_file:
            if line.startswith('v '):
                _, x, y, z = line.split()
                vertices.append([float(x), float(y), float(z)])
    
    with open(json_filename, 'w') as f:
        json.dump(vertices, f, indent=4)

# Example usage
obj_file_path = './kera.obj'
json_filename = "output1.json"
convert_obj_to_js_array(obj_file_path, json_filename)
