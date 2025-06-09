# PS5-BDJ-HEN-loader

Load etaHEN and/or kstuff from inside the PS5's Blu-ray player automatically, without having to exit the Disc Player manually. It will all be done automatically. [Viktorious X](https://x.com/Viktorioussssss) already has this integrated in his [ISO](https://github.com/Viktorious-x/ps5-bdjb-modified-ISOs).

> [!NOTE]
> Sometimes it seems to be stuck on the homescreen after closing the Disc Player. In this case, just wait until you see the "Successfully killed DiscPlayer" notification. This can take up a few seconds.

> [!IMPORTANT]  
> Sometimes the Disc Player is crashing when exiting. Now you can get a kernel panic or the message that "something went wrong with this game or app". In this case, just proceed by clicking OK, and everything should load successfully.

---

### Configurations:

There are multiple configurations you can use:

1. **Default: etaHEN** will be loaded from the **disc**, and you don't have to change anything.
2. **etaHEN only:** Copy an `etaHEN.bin` file to the **root of a USB drive** (priority 1) **or** to the **/data/ directory** (priority 2) on the PS5, and it will be **loaded instead of the one from the disc**.
3. **Kstuff only:** Same as above, but with kstuff. Copy a `kstuff.elf` to the **root of a USB drive** (priority 1) or to the **/data/ directory** (priority 2) on the PS5, and it will be **loaded instead of etaHEN**.
4. **Combined:** Place both `etaHEN.bin` and `kstuff.elf` on the **root of a USB drive** (priority 1) or in the **/data/ directory** (priority 2) on the PS5, and place a `no_kstuff` file on the **USB root** or in **/data/etaHEN/**. Now etaHEN will be loaded, followed by kstuff. <br> It is also supported for etaHEN and kstuff to be in different locations. For example, etaHEN can be on the USB and kstuff in the /data/ directory.

---

### Building (on Ubuntu):
Clone this repository and enter it: <br>

```sh
sudo apt update
sudo apt install -y git 
git clone https://github.com/BenNoxXD/PS5-BDJ-HEN-loader
cd PS5-BDJ-HEN-loader
```

#### From scratch (optional): 
Compile [hammer-83's ps5-jar-loader](https://github.com/hammer-83/ps5-jar-loader) and copy the `Status.class` located here: `ps5-jar-loader/xlet/target/classes/org/ps5jb/loader/Status.class`. Now you can paste this file here `HENloader_Java_part/org/ps5jb/loader/Status.class`. <br>

```sh
sudo apt update
sudo apt install -y openjdk-11-jdk maven
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH
git clone https://github.com/hammer-83/ps5-jar-loader
cd ps5-jar-loader
mvn clean package
cd ..
cp -r ps5-jar-loader/xlet/target/classes/org/ps5jb/loader/Status.class HENloader_Java_part/org/ps5jb/loader/
```

#### Build the payloads: <br>
##### Install dependencies:  <br>
```sh
sudo apt-get update
sudo apt-get install -y bash clang-15 lld-15 cmake wget unzip openjdk-11-jdk
wget https://github.com/ps5-payload-dev/sdk/releases/latest/download/ps5-payload-sdk.zip
sudo unzip -d /opt ps5-payload-sdk.zip
rm ps5-payload-sdk.zip
export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH
```

##### Build:
```sh
make -C HENloader_C_part/
cd HENloader_Java_part
rm HENloader.class && rm HENloader.jar
cp ../HENloader_C_part/*.elf .
javac HENloader.java
jar cfm HENloader.jar MANIFEST.MF *.class org/ps5jb/loader/*.class *.elf
```

---

### Credits

* Huge thanks to [Dr.Oid (X ](https://x.com/RepassyMate) [/ YouTube)](https://www.youtube.com/@Dr.Oid27), the developer who made the original etaHEN.bin JAR sender, whose source is also used in this project. Without him, this would have been much more complicated.
* Big thanks to [Viktorious X](https://x.com/Viktorioussssss) for pointing out the initial problem, extensive testing, his general help, and for maintaining [the ISO](https://github.com/Viktorious-x/ps5-bdjb-modified-ISOs).

Thanks to everyone else in the scene who made all of this possible in the first place:

* [Andy Nguyen / theflow0](https://x.com/theflow0) — for finding the BDJ vulnerability
* [hammer-83](https://github.com/hammer-83) — ps5-jar-loader
* [iakdev](https://github.com/iakdev) — USB loader and pipeline loader
* [cryonumb](https://github.com/cryonumb) — BDJ ELF loader
* [sb / John Törnblom](https://github.com/john-tornblom) — elfldr and the ps5-payload-sdk
* [Lightning Mods](https://github.com/LightningMods) — etaHEN
* [sleirsgoevy](https://github.com/sleirsgoevy) & [Echo Stretch](https://x.com/StretchEcho) — kstuff
* [driverick](https://github.com/DriveRick)
