---
title: Setting Up Nuclei
draft: false
tags:
  - nuclei
---
## Install Nuclei

Nuclei requires **go1.21** to install successfully. Run the following command to install the latest version:

```bash
go env -w GOPROXY=https://goproxy.cn,direct
go env GOPROXY
go install -v github.com/projectdiscovery/nuclei/v3/cmd/nuclei@latest
```


`docker pull projectdiscovery/nuclei:latest`


## Usage
| Option             | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `-u`, `-target`    | Specify scan target                                                         |
| `-l`, `-list`      | Specify a file containing a list of scan targets                            |
| `-t`, `-templates` | Specify vulnerability scan templates and folders                           |
| `-V`, `-var`       | Specify variables used in templates                                         |
| `-debug`           | Output all Request and Response data                                        |
| `-headless`        | Run the browser in the background when performing behavior-based scans (e.g., Selenium) |
| `-sb`, `-show-browser` | Run the browser in the foreground when performing behavior-based scans (e.g., Selenium) |
| `-update`          | Update the Nuclei engine                                                    |
| `-ut`, `-update-templates` | Update Nuclei templates                                             |



## References
- https://github.com/projectdiscovery/nuclei
