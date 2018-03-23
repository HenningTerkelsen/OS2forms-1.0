# OS2forms-1.0
A Drupal distro for advanced webforms in Danish Municipalities

## Contributing municipalities
* Ballerup Kommune

## Documentation
Documentation wiki (https://github.com/OS2Forms/OS2forms-1.0/wiki)

## OS2forms Submodules

os2forms_frontend (https://github.com/OS2Forms/os2forms_frontend)

egbolig_webform (https://github.com/OS2Forms/egbolig_webform)

borger_formularer (https://github.com/OS2Forms/borger_formularer)

os2forms_settings (https://github.com/OS2Forms/os2forms_settings)

os2forms_backend (https://github.com/OS2Forms/os2forms_backend


## OS2forms modules in process to be contributed to drupal.org

drupal_nemid (https://github.com/bellcom/drupal_nemid)


## Project management

The project backlog / tasks and planing progress can be followed here in jira. 

https://os2web.atlassian.net/secure/RapidBoard.jspa?rapidView=30

## Project setup

Clone repo using folloing command
```
git clone https://github.com/OS2Forms/OS2forms-1.0
```
Add submodules using folloing commands
```
git submodule init
git submodule ubdate
```
Install project by accessing to url fe. os2forms.dk/install.php

Enable requierd modules 

For backend server: os2forms_backend
For frontent server: os2forms_frontend, egbolig_webform, borger_formularer
