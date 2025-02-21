import os
import requests
from tqdm import tqdm
from requests_toolbelt.multipart.encoder import MultipartEncoder, MultipartEncoderMonitor

def upload_firmware(file_path, url, username, password):
    # Check if the file exists
    if not os.path.isfile(file_path):
        print(f"Error: File '{file_path}' not found.")
        return

    # Open the file in binary mode
    file_size = os.path.getsize(file_path)
    file_name = os.path.basename(file_path)

    try:
        # Create a multipart encoder for form-data
        with open(file_path, "rb") as file:
            def create_callback(encoder):
                # Create a callback function for the progress bar
                progress_bar = tqdm(total=file_size, unit="B", unit_scale=True, desc="Uploading", ncols=100)

                def callback(monitor):
                    progress_bar.update(monitor.bytes_read - progress_bar.n)

                return callback

            # Prepare the multipart encoder
            encoder = MultipartEncoder(
                fields={"file": (file_name, file, "application/octet-stream")}
            )

            # Attach the progress bar callback
            monitor = MultipartEncoderMonitor(encoder, create_callback(encoder))

            # Send the POST request with basic authentication
            response = requests.post(
                url,
                auth=(username, password),
                headers={"Content-Type": monitor.content_type},
                data=monitor,
                timeout=10  # Timeout after 10 seconds
            )

            # Close the progress bar
            create_callback(encoder)(monitor)  # Final update to ensure the bar reaches 100%

            # Check the response status
            if response.status_code == 200:
                print("\nUpload successful!")
            else:
                # Print detailed error message from the response
                try:
                    response_data = response.json()
                    error_message = response_data.get("message", "Unknown error")
                except ValueError:
                    error_message = response.text.strip() or "No error message provided"

                print(f"\nUpload failed with status code {response.status_code}: {error_message}")

    except requests.exceptions.RequestException as e:
        # Handle network-related errors (e.g., connection issues, timeouts)
        print(f"\nAn error occurred during the upload: {e}")
    except Exception as e:
        # Catch any other unexpected exceptions
        print(f"\nAn unexpected error occurred: {e}")

if __name__ == "__main__":
    # Configuration
    firmware_file = "./.pio/build/esp-fan-hub/firmware.bin"  # Replace with the path to your firmware file
    upload_url = "http://192.168.1.183/update"
    username = "user"
    password = "123456"

    # Start the upload process
    upload_firmware(firmware_file, upload_url, username, password)