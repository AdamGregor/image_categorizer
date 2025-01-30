# image_categorizer
simple project cotegorizig images into few cathergories

To work, files from https://drive.google.com/drive/folders/1R6G0nvoULnODu3XMP786sj-iGgCddieI?usp=sharing must be placed into application/resources.
To build the docker image, run the docker_build.sh. To run the image, run the docker_run.sh. 
After attached to the container, it should be posssible to build the project using cmake and run it.

The program searches the path spefified in application/resources.config.json:input_folder for images, classifies its content and moves it into subdirectories
specified in application/resources.config.json:output_folder
