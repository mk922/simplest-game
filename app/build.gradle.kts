plugins {
    id("com.android.application")
}
android {
    namespace = "io.github.mk922.simplestgame"
    compileSdk = 37
    defaultConfig {
        applicationId = "io.github.mk922.simplestgame"
        minSdk = 30
        targetSdk = 37
        versionCode = 1
        versionName = "1.0"
        externalNativeBuild {
            cmake {
                arguments("-DCMAKE_BUILD_TYPE=Release")
            }
        }
        ndk {
            abiFilters.add("arm64-v8a")
        }
    }
    buildTypes {
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt")
            )
            ndk {
                debugSymbolLevel = "none"
            }
        }
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}
