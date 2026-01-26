ARG VARIANT=trixie
FROM mcr.microsoft.com/vscode/devcontainers/base:${VARIANT}

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get -y install --no-install-recommends python3-venv
RUN groupadd -g 18 dialout_fedora
RUN usermod -aG dialout,dialout_fedora vscode

# Install PlatformIO CLI
USER vscode
RUN curl -fsSL -o /tmp/get-platformio.py \
      https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py

RUN python3 /tmp/get-platformio.py

RUN echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' | \
      tee -a /home/vscode/.bashrc /home/vscode/.zshrc

RUN echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' | \
      sudo tee -a /root/.bashrc /root/.zshrc