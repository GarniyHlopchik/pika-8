## Android building

Pika now supports building for android devices. First, you need to get the base .apk file of the engine. If you want to build it from source, you need to have basic android sdk tools for building android apps: build-tools, cmake, ndk installed and added to path, as well as a version of jdk. Once that is done go to pika-android directory and write commands 

`.\gradlew.bat clean`

`.\gradlew.bat assembleDebug`

if building is successful, it will generate the app-debug.apk in app/build/outputs/apk/debug

this apk is just a template that doesn't have any game in it yet. You can then use android_build.py script in build_scripts folder to embed your game files, icon and game name into it. Open the script, edit configuration data so the script can find necessary paths to android sdk, .jdk keystore and apktool, which is necessary for building your game:

```python
# --- CONFIGURATION (Update these paths) ---
APKTOOL_JAR = r"D:\soft\apktool\apktool_3.0.1.jar"
SDK_BUILD_TOOLS = r"D:\soft\AndroidSDK\build-tools\34.0.0"
KEYSTORE_PATH = r"D:\projects\c++\pika8\pika8-release.jks"
KEYSTORE_ALIAS = "pika8_key"
KEYSTORE_PASS = "pass:YOURKEYSTOREPASSWORDHERE"
```

Now go to the end of the file and pass the necessary paths to your template apk, game.pika, png icon and your game name:

```python
if __name__ == "__main__":
    # Usage Example:
    export_game(
        template_apk="app-debug.apk",
        game_pika="game.pika",
        game_name="My Game",
        icon_path="my_icon.png"
    )
```

now run the script with `python android_build.py` and it will generate a final .apk file of your game.