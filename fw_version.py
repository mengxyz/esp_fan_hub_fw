import subprocess

def get_git_info():
    try:
        # Get the current Git commit hash
        git_hash = subprocess.check_output(['git', 'rev-parse', 'HEAD']).strip().decode('utf-8')
        
        # Get the current Git tag name, if any
        try:
            git_tag = subprocess.check_output(['git', 'describe', '--tags', '--abbrev=0']).strip().decode('utf-8')
        except subprocess.CalledProcessError:
            git_tag = "0.0.0"
        
        return f"{git_tag} - {git_hash}"
    except subprocess.CalledProcessError:
        return "unknown - unknown"

def set_build_flags(env):
    git_info = get_git_info()
    print("Current Git Info: ", git_info)
    env.Append(CPPDEFINES=[('FW_VERSION', '\\"{}\\"'.format(git_info))])

# Add the function to the extra_scripts
Import("env")
set_build_flags(env)
