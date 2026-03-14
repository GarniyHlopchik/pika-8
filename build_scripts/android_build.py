import os
import shutil
import subprocess
import xml.etree.ElementTree as ET

# --- CONFIGURATION (Update these paths) ---
APKTOOL_JAR = r"D:\soft\apktool\apktool_3.0.1.jar"
SDK_BUILD_TOOLS = r"D:\soft\AndroidSDK\build-tools\34.0.0"
KEYSTORE_PATH = r"D:\projects\c++\pika8\pika8-release.jks"
KEYSTORE_ALIAS = "pika8_key"
KEYSTORE_PASS = "pass:YOURKEYSTOREPASSWORDHERE"

def run_cmd(cmd):
    print(f"Executing: {' '.join(cmd)}")
    subprocess.run(cmd, check=True)

def export_game(template_apk, game_pika, game_name, icon_path=None):
    # Sanitize game name for package ID (no spaces/special chars)
    safe_name = "".join(filter(str.isalnum, game_name.lower()))
    new_package_id = f"com.pika8.{safe_name}"
    
    work_dir = "temp_work"
    unsigned_apk = "rebuilt_unsigned.apk"
    aligned_apk = "rebuilt_aligned.apk"
    final_apk = f"{game_name}.apk"

    # 1. DECODE TEMPLATE
    if os.path.exists(work_dir): shutil.rmtree(work_dir)
    run_cmd(["java", "-jar", APKTOOL_JAR, "d", template_apk, "-o", work_dir, "-f"])

    # 2. INJECT game.pika
    assets_dir = os.path.join(work_dir, "assets")
    os.makedirs(assets_dir, exist_ok=True)
    shutil.copy2(game_pika, os.path.join(assets_dir, "game.pika"))

    # 3. MODIFY APP NAME (strings.xml)
    strings_path = os.path.join(work_dir, "res", "values", "strings.xml")
    tree = ET.parse(strings_path)
    root = tree.getroot()
    for string_tag in root.findall("string"):
        if string_tag.get("name") == "app_name":
            string_tag.text = game_name
    tree.write(strings_path, encoding="utf-8", xml_declaration=True)

    # 4. MODIFY APPLICATION ID (AndroidManifest.xml)
    manifest_path = os.path.join(work_dir, "AndroidManifest.xml")
    # Note: Apktool decodes manifest. We need to replace the package attribute.
    with open(manifest_path, 'r', encoding='utf-8') as f:
        content = f.read()
    content = content.replace('package="com.pika8.engine"', f'package="{new_package_id}"')
    with open(manifest_path, 'w', encoding='utf-8') as f:
        f.write(content)

    # 5. REPLACE ICON (Optional)
    if icon_path and os.path.exists(icon_path):
        # Android has icons in multiple densities. We'll overwrite the main ones.
        # This assumes your template uses @mipmap/ic_launcher
        mipmap_dirs = [d for d in os.listdir(os.path.join(work_dir, "res")) if "mipmap" in d]
        for md in mipmap_dirs:
            target_icon = os.path.join(work_dir, "res", md, "ic_launcher.png")
            if os.path.exists(target_icon):
                shutil.copy2(icon_path, target_icon)

    # 6. REBUILD
    run_cmd([
        "java", "-jar", APKTOOL_JAR, "b", work_dir, "-o", unsigned_apk,
        "--aapt", os.path.join(SDK_BUILD_TOOLS, "aapt2.exe")
    ])

    # 7. ZIPALIGN
    run_cmd([os.path.join(SDK_BUILD_TOOLS, "zipalign"), "-f", "4", unsigned_apk, aligned_apk])

    # 8. SIGN
    run_cmd([
        os.path.join(SDK_BUILD_TOOLS, "apksigner.bat"), "sign", 
        "--ks", KEYSTORE_PATH,
        "--ks-pass", KEYSTORE_PASS,
        "--ks-key-alias", KEYSTORE_ALIAS,
        "--out", final_apk,
        aligned_apk
    ])

    # CLEANUP
    os.remove(unsigned_apk)
    os.remove(aligned_apk)
    shutil.rmtree(work_dir)
    print(f"\nSUCCESS! Created: {final_apk}")

if __name__ == "__main__":
    # Usage Example:
    export_game(
        template_apk="app-debug.apk",
        game_pika="game.pika",
        game_name="Neo Racist",
        icon_path="my_icon.png"
    )