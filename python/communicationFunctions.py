#Libraries:

import serial
from PIL import Image
from io import BytesIO
import pandas as pd

def resize_image(input_image, output_image, target_resolution=(640, 480)):
    try:
        # Open the input image
        with Image.open(input_image) as img:
            # Resize the image
            resized_img = img.resize(target_resolution)

            # Save the resized image
            resized_img.save(output_image)
            print(f"Image resized and saved to {output_image}")

    except Exception as e:
        print(f"Error: {e}")

def convert_to_3bit_grayscale(input_image, output_image):
    try:
        # Open the input image
        with Image.open(input_image) as img:
            # Convert the image to 3-bit grayscale
            three_bit_img = img.convert('L', palette=Image.ADAPTIVE, colors=8)

            # Save the 3-bit grayscale image
            three_bit_img.save(output_image)
            print(f"Image converted to 3-bit grayscale and saved to {output_image}")

    except Exception as e:
        print(f"Error: {e}")

def image_to_dataframe_3bit_inverted(image_path):
    try:
        # Open the 3-bit grayscale image
        with Image.open(image_path) as img:
            # Convert the image to 3-bit grayscale
            img = img.convert('L', palette=Image.ADAPTIVE, colors=8)

            # Get pixel data
            pixels = list(img.getdata())

            # Get image size
            width, height = img.size

            # Create a DataFrame with pixel information
            df = pd.DataFrame(pixels, columns=["Pixel_Value"])

            # Invert the pixel values
            df["Pixel_Value"] = 7 - (df["Pixel_Value"] // 32)

            # Add columns for X and Y coordinates
            df["X"] = [i % width for i in range(len(pixels))]
            df["Y"] = [i // width for i in range(len(pixels))]

            print("DataFrame created successfully.")
            return df

    except Exception as e:
        print(f"Error: {e}")
        return None

def export_dataframe_to_excel(dataframe, output_excel_path):
    try:
        # Export the DataFrame to Excel
        dataframe.to_excel(output_excel_path, index=False)

        print(f"DataFrame exported to {output_excel_path}")

    except Exception as e:
        print(f"Error: {e}")

def clean_and_organize_data(pixel_df):
    try:
        # Clean the data
        cleaned_df = pixel_df[pixel_df.groupby('Y')['Pixel_Value'].transform('sum') > 0]

        # Organize pixel values into a matrix
        organized_matrix = cleaned_df.pivot(index='Y', columns='X', values='Pixel_Value')

        return organized_matrix

    except Exception as e:
        print(f"Error: {e}")
        return None

    