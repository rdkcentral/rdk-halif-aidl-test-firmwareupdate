#!/usr/bin/env python3
#** *****************************************************************************
# *
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2026 RDK Management
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#* ******************************************************************************

import yaml

class virtualFirmwareUpdate:
    """Send FirmwareUpdate L3 stimulus commands to a vDevice control plane."""

    def __init__(self, session, prompt: str = "~#", port: int = 8087):
        self.session = session
        self.prompt = prompt
        self.port = port

    def _sendCommand(self, payload: dict):
        yamlString = yaml.dump(payload)
        temporaryFile = "/tmp/firmwareupdate_cp.yaml"

        heredocLines = [f"cat > {temporaryFile} << 'YAML_EOF'"]
        heredocLines.extend(yamlString.rstrip("\n").splitlines())
        heredocLines.append("YAML_EOF")
        self.session.write(heredocLines)
        self.session.write(
            f'curl -s -X POST -H "Content-Type: application/x-yaml" '
            f'--data-binary @{temporaryFile} "http://localhost:{self.port}/api/postKVP"'
        )

    def injectFirmwareUpdateResult(self, result: str):
        """Set the result returned by the virtual FirmwareUpdate component."""
        self._sendCommand({
            "FirmwareUpdate": {
                "command": "inject_firmware_update_result",
                "result": result,
            }
        })