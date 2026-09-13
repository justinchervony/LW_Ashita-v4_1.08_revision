# Lootwhore
Lootwhore is a plugin that allows you to automate lotting, passing, dropping items from inventory, and moving stacks of items from inventory to other bags. This is done through a profile file, which can be edited manually (see *ashita/docs/lootwhore/profilexmlstructure.xml*) or by using typed commands ingame. Profiles should be stored in *ashita/config/lootwhore/profiles*. Lootwhore also has a configuration file that allows you to tweak several settings that cannot be changed from inside the game (see *ashita/docs/lootwhore/configxmlstructure.xml*). Configuration files should be stored as *ashita/config/lootwhore/default.xml* or *ashita/config/lootwhore/Playername.xml*. Lootwhore will prefer a player-specific config file to the default file, allowing you to change settings for each character.

## Commands
All commands can be prefixed with **/lw** or **/lootwhore**.<br>
Any parameter that includes a space must be enclosed in quotation marks(**"**).

**/lw profile [Required: Profile Name]**<br>
Load a profile, containing settings to determine how items should be treated.

**/lw export [Optional: Profile Name]**<br>
Saves the current settings to a profile. If you already have a profile loaded, you can omit name to overwrite it.

**/lw reset**<br>
Resets all profile settings. This does not reset configuration.

**/lw default [Required: lot/pass/ignore]**<br>
Sets the default action to be performed on items in treasure pool.

**/lw smartpass [Required: everyone/listonly/off]**<br>
Changes smartpass setting. If smartpass is enabled, lootwhore will pass whenever someone else lots an item. If it is in list only mode, this only applies to people on your whitelist in your configuration file.

**/lw rarepass [Required: on/off]**<br>
Changes rarepass setting. If rarepass is enabled, lootwhore will automatically pass anything rare marked that you already have.

**/lw autostack [Required: on/off]**<br>
Changes autostack setting. While autostack is enabled, items in your inventory will be automatically stacked together whenever space can be saved by doing so. Autostack defaults to off and persists per character profile. Use **/lw export** after toggling to save the setting.

**/lw addstackignore [Required: item id or name]**<br>
Adds an item to the autostack ignore list. Items on this list will never be automatically stacked, even when autostack is enabled. Useful for items you want to keep as separate stacks.

**/lw removestackignore [Required: item id or name]**<br>
Removes an item from the autostack ignore list.

**/lw zonereset [Required: on/off]**<br>
Changes zonereset setting. If zonereset is enabled, your profile will be reset the next time you zone.<br>
Note that the act of resetting profile also clears zonereset, so enabling it will only result in a single reset. This is recommended for safety when using default pass.

**/lw add [Required: item id or name] [Required: ignore/lot/pass]**<br>
Adds an item-specific reaction to be performed on items in treasure pool.

**/lw remove [Required: item id or name]**<br>
Clears an item-specific reaction from an item.

**/lw adddrop [Required: item id or name]**<br>
Adds an item to drop list to be automatically dropped whenever it enters inventory.

**/lw removedrop [Required: item id or name]**<br>
Removes an item from drop list.

**/lw addstore [Required: item id or name]**<br>
Adds an item to store list to be automatically stored whenever a full stack is in inventory. You can change which bags lootwhore will store to in your configuration xml.

**/lw removestore [Required: item id or name]**<br>
Removes an item from store list.

**/lw list [Optional: react/drop/store]**<br>
Lists your current item settings. If you specify a type, will only list that type.

**/lw search [Required: partial item name]**<br>
Searches the item database by name and returns matching item IDs. Returns up to 10 results. Useful for finding item IDs for custom or unusually named items. Spaces in search terms are supported.

**/lw lot**<br>
Lots all items you have not yet passed in current treasure pool.

**/lw pass**<br>
Passes all items you have not yet lotted in current treasure pool.

**/lw help [Optional: command]**<br>
Print information on a command. If no command is specified, a list of commands will be printed.
