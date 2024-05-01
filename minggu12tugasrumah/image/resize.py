from PIL import Image
import os

def resize_image(input_image_path, output_image_path, size):
    original_image = Image.open(input_image_path)
    original_image.thumbnail((size, size), Image.ANTIALIAS)
    if not os.path.exists(os.path.dirname(output_image_path)):
        os.makedirs(os.path.dirname(output_image_path))
    original_image.save(output_image_path)

prefix = "./texture/"
textureImages = [
    "cobblestone.jpg",
    "diamond.jpg",
    "dirt.jpg",
    "emerald.jpg",
    "iron.jpg",
    "wood.jpg",
]
for i in range(len(textureImages)):
    input_path = textureImages[i]
    output_path = "resize/resize_" + textureImages[i] + ".jpg"
    target_size = 512
    resize_image(prefix+input_path, output_path, target_size)
