# Host-only workaround for an upstream simulator static-initialization crash.
Import('env')

from pathlib import Path
import subprocess

project = Path(env.subst("$PROJECT_DIR"))
dep = project / '.pio' / 'libdeps' / env['PIOENV'] / 'simulator'
patch = project / 'scripts' / 'simulator-simlog-init.patch'
expected = '6730fe5fe576c9d924e44096a9e267d8538d184c'


def git(*args):
    return subprocess.run(['git', '-C', str(dep), *args], capture_output=True, text=True)


head = git('rev-parse', 'HEAD')
if head.returncode or head.stdout.strip() != expected:
    raise RuntimeError(f'Unexpected simulator revision at {dep}: {head.stdout.strip()} {head.stderr.strip()}')
if git('apply', '--unidiff-zero', '--reverse', '--check', str(patch)).returncode == 0:
    print('Simulator compatibility patch already applied')
else:
    check = git('apply', '--unidiff-zero', '--check', str(patch))
    if check.returncode:
        raise RuntimeError(f'Simulator compatibility patch cannot apply: {check.stderr.strip()}')
    apply = git('apply', '--unidiff-zero', str(patch))
    if apply.returncode:
        raise RuntimeError(f'Simulator compatibility patch failed: {apply.stderr.strip()}')
    print('Applied simulator static-initialization compatibility patch')
