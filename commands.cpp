#include "Lootwhore.h"
#include <algorithm>

bool Lootwhore::HandleCommand(int32_t mode, const char* command, bool injected)
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<string> args;
    int argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (CheckArg(0, "/lw") || CheckArg(0, "/lootwhore"))
    {
        if (argcount < 2)
        {
            HandleCommandHelp(args, argcount, mCommandMap["help"].help);
            return true;
        }

        auto iter = mCommandMap.find(args[1]);
        if (iter == mCommandMap.end())
        {
            pOutput->error_f("Command not recognized. [$H%s$R]", args[1].c_str());
            return true;
        }
        (this->*(iter->second.handler))(args, argcount, iter->second.help);
        return true;
    }

    return false;
}

void Lootwhore::HandleCommandProfile(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
    }
    else
    {
        LoadProfile(args[2].c_str());
    }
}
void Lootwhore::HandleCommandExport(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        if (mState.CurrentProfile == "NO_FILE")
            PrintHelpText(help, true);
        else
            SaveProfile(mState.CurrentProfile.c_str(), false);

        return;
    }
    SaveProfile(args[2].c_str(), true);
}
void Lootwhore::HandleCommandReset(std::vector<string> args, int argcount, CommandHelp help)
{
    mProfile              = Profile_t();
    mState.CurrentProfile = "NO_FILE";
    pOutput->message("Profile cleared.");
}
void Lootwhore::HandleCommandDefault(std::vector<string> args, int argcount, CommandHelp help)
{
    LotReaction reaction = LotReaction::Unknown;
    if (CheckArg(2, "lot"))
        reaction = LotReaction::Lot;
    else if (CheckArg(2, "pass"))
        reaction = LotReaction::Pass;
    else if (CheckArg(2, "ignore"))
        reaction = LotReaction::Ignore;
    else
    {
        PrintHelpText(help, true);
        return;
    }

    mProfile.DefaultReaction = reaction;
    pOutput->message_f("Default reaction set to $H%s$R.", args[2].c_str());
}
void Lootwhore::HandleCommandSmartPass(std::vector<string> args, int argcount, CommandHelp help)
{
    if (CheckArg(2, "everyone"))
        mProfile.SmartPass = SmartPassSetting::Everyone;
    else if (CheckArg(2, "listonly"))
        mProfile.SmartPass = SmartPassSetting::ListOnly;
    else
        mProfile.SmartPass = SmartPassSetting::Disabled;

    if (mProfile.SmartPass == SmartPassSetting::Everyone)
        pOutput->message("Smartpass set to $Heveryone$R.");
    else if (mProfile.SmartPass == SmartPassSetting::ListOnly)
        pOutput->message("Smartpass set to $Hlist only$R.");
    else
        pOutput->message("Smartpass set to $Hdisabled$R.");
}
void Lootwhore::HandleCommandRarePass(std::vector<string> args, int argcount, CommandHelp help)
{
    if (CheckArg(2, "on"))
        mProfile.RarePass = true;
    else
        mProfile.RarePass = false;
    pOutput->message_f("Rarepass $H%s$R.", mProfile.RarePass ? "enabled" : "disabled");
}
void Lootwhore::HandleCommandAutoStack(std::vector<string> args, int argcount, CommandHelp help)
{
    if (CheckArg(2, "on"))
        mProfile.AutoStack = true;
    else
        mProfile.AutoStack = false;
    pOutput->message_f("Autostack $H%s$R.", mProfile.AutoStack ? "enabled" : "disabled");
}
void Lootwhore::HandleCommandZoneReset(std::vector<string> args, int argcount, CommandHelp help)
{
    if (CheckArg(2, "on"))
        mProfile.ResetOnZone = true;
    else
        mProfile.ResetOnZone = false;
    pOutput->message_f("ZoneReset $H%s$R.", mProfile.ResetOnZone ? "enabled" : "disabled");
}
void Lootwhore::HandleCommandAdd(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 4)
    {
        PrintHelpText(help, true);
        return;
    }

    LotReaction reaction = LotReaction::Unknown;
    if (CheckArg(3, "lot"))
        reaction = LotReaction::Lot;
    else if (CheckArg(3, "pass"))
        reaction = LotReaction::Pass;
    else if (CheckArg(3, "ignore"))
        reaction = LotReaction::Ignore;
    else
    {
        PrintHelpText(help, true);
        return;
    }

    IItem* item = NULL;
    if (IsPositiveInteger(args[2]))
        item = m_AshitaCore->GetResourceManager()->GetItemById(atoi(args[2].c_str()));
    else
        item = m_AshitaCore->GetResourceManager()->GetItemByName(args[2].c_str(), 0);

    if ((item == NULL) || (strlen(item->Name[0]) < 3))
    {
        PrintHelpText(help, true);
        return;
    }

    mProfile.ItemMap[item->Id] = reaction;
    pOutput->message_f("$H%s$R set to $H%s$R.", item->Name[0], args[3].c_str());
}
void Lootwhore::HandleCommandRemove(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    uint16_t id = 0;
    if (IsPositiveInteger(args[2]))
    {
        int tempId = atoi(args[2].c_str());
        if (tempId < 65535)
            id = (uint16_t)tempId;
    }

    int EraseCount = 0;
    for (std::map<uint16_t, LotReaction>::iterator find = mProfile.ItemMap.begin(); find != mProfile.ItemMap.end();)
    {
        if (id == find->first)
        {
            find = mProfile.ItemMap.erase(find);
            EraseCount++;
        }
        else
        {
            IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(find->first);
            if (_stricmp(pResource->Name[0], args[2].c_str()) == 0)
            {
                find = mProfile.ItemMap.erase(find);
                EraseCount++;
            }
            else
                find++;
        }
    }

    if (EraseCount == 0)
        pOutput->error_f("Could not find a matching item to remove from reaction list.  [$H%s$R]", args[2].c_str());
    else if (EraseCount == 1)
        pOutput->message_f("$H%s$R removed from reaction list.", args[2].c_str());
    else
        pOutput->message_f("$H%d$R items matching $H%s$R were removed from reaction list.", EraseCount, args[2].c_str());
}
void Lootwhore::HandleCommandAddDrop(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    IItem* item = NULL;
    if (IsPositiveInteger(args[2]))
        item = m_AshitaCore->GetResourceManager()->GetItemById(atoi(args[2].c_str()));
    else
        item = m_AshitaCore->GetResourceManager()->GetItemByName(args[2].c_str(), 0);

    if ((item == NULL) || (strlen(item->Name[0]) < 3))
    {
        PrintHelpText(help, true);
        return;
    }

    mProfile.AutoDrop.push_back((uint16_t)item->Id);
    pOutput->message_f("Added $H%s$R to drop list.", item->Name[0]);
}
void Lootwhore::HandleCommandRemoveDrop(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    uint16_t id = 0;
    if (IsPositiveInteger(args[2]))
    {
        int tempId = atoi(args[2].c_str());
        if (tempId < 65535)
            id = (uint16_t)tempId;
    }

    int EraseCount = 0;

    for (std::list<uint16_t>::iterator iter = mProfile.AutoDrop.begin(); iter != mProfile.AutoDrop.end();)
    {
        if (id == *iter)
        {
            iter = mProfile.AutoDrop.erase(iter);
            EraseCount++;
        }
        else
        {
            IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(*iter);
            if (_stricmp(pResource->Name[0], args[2].c_str()) == 0)
            {
                iter = mProfile.AutoDrop.erase(iter);
                EraseCount++;
            }
            else
                iter++;
        }
    }

    if (EraseCount == 0)
        pOutput->error_f("Could not find a matching item to remove from drop list.  [$H%s$R]", args[2].c_str());
    else if (EraseCount == 1)
        pOutput->message_f("$H%s$R removed from drop list.", args[2].c_str());
    else
        pOutput->message_f("$H%d$R items matching $H%s$R were removed from drop list.", EraseCount, args[2].c_str());
}
void Lootwhore::HandleCommandAddStore(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    IItem* item = NULL;
    if (IsPositiveInteger(args[2]))
        item = m_AshitaCore->GetResourceManager()->GetItemById(atoi(args[2].c_str()));
    else
        item = m_AshitaCore->GetResourceManager()->GetItemByName(args[2].c_str(), 0);

    if ((item == NULL) || (strlen(item->Name[0]) < 3))
    {
        PrintHelpText(help, true);
        return;
    }

    mProfile.AutoStore.push_back((uint16_t)item->Id);
    pOutput->message_f("Added $H%s$R to store list.", item->Name[0]);
}
void Lootwhore::HandleCommandRemoveStore(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    uint16_t id = 0;
    if (IsPositiveInteger(args[2]))
    {
        int tempId = atoi(args[2].c_str());
        if (tempId < 65535)
            id = (uint16_t)tempId;
    }

    int EraseCount = 0;

    for (std::list<uint16_t>::iterator iter = mProfile.AutoStore.begin(); iter != mProfile.AutoStore.end();)
    {
        if (id == *iter)
        {
            iter = mProfile.AutoStore.erase(iter);
            EraseCount++;
        }
        else
        {
            IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(*iter);
            if (_stricmp(pResource->Name[0], args[2].c_str()) == 0)
            {
                iter = mProfile.AutoStore.erase(iter);
                EraseCount++;
            }
            else
                iter++;
        }
    }

    if (EraseCount == 0)
        pOutput->error_f("Could not find a matching item to remove from store list.  [$H%s$R]", args[2].c_str());
    else if (EraseCount == 1)
        pOutput->message_f("$H%s$R removed from store list.", args[2].c_str());
    else
        pOutput->message_f("$H%d$R items matching $H%s$R were removed from store list.", EraseCount, args[2].c_str());
}
void Lootwhore::HandleCommandList(std::vector<string> args, int argcount, CommandHelp help)
{
    bool PrintDrop  = true;
    bool PrintReact = true;
    bool PrintStore = true;
    if (CheckArg(2, "react"))
    {
        PrintDrop  = false;
        PrintStore = false;
    }
    else if (CheckArg(2, "drop"))
    {
        PrintReact = false;
        PrintStore = false;
    }
    else if (CheckArg(2, "store"))
    {
        PrintDrop  = false;
        PrintReact = false;
    }
    else if (argcount > 2)
    {
        PrintHelpText(help, true);
        return;
    }

    if (PrintReact)
    {
        pOutput->message("Reaction List");
        for (std::map<uint16_t, LotReaction>::iterator iter = mProfile.ItemMap.begin(); iter != mProfile.ItemMap.end(); iter++)
        {
            IItem* item        = m_AshitaCore->GetResourceManager()->GetItemById(iter->first);
            std::string Action = "Ignore";
            if (iter->second == LotReaction::Lot)
                Action = "Lot";
            else if (iter->second == LotReaction::Pass)
                Action = "Pass";

            pOutput->message_f("$H%s$R : $H%s$R", item->Name[0], Action.c_str());
        }
    }

    if (PrintDrop)
    {
        pOutput->message("Drop List");
        for (std::list<uint16_t>::iterator iter = mProfile.AutoDrop.begin(); iter != mProfile.AutoDrop.end(); iter++)
        {
            IItem* item = m_AshitaCore->GetResourceManager()->GetItemById(*iter);
            pOutput->message_f("$H%s$R : $HDrop$R", item->Name[0]);
        }
    }

    if (PrintStore)
    {
        pOutput->message("Store List");
        for (std::list<uint16_t>::iterator iter = mProfile.AutoStore.begin(); iter != mProfile.AutoStore.end(); iter++)
        {
            IItem* item = m_AshitaCore->GetResourceManager()->GetItemById(*iter);
            pOutput->message_f("$H%s$R : $HStore$R", item->Name[0]);
        }
    }
}
void Lootwhore::HandleCommandLot(std::vector<string> args, int argcount, CommandHelp help)
{
    int LotCount = 0;

    for (int x = 0; x < 10; x++)
    {
        if (mState.PoolSlots[x].Id == 0)
            continue;
        if (mState.PoolSlots[x].Status != LotState::Untouched)
            continue;
        if (mState.PoolSlots[x].PacketAttempts >= mSettings.MaxRetry)
            continue;
        if (std::chrono::steady_clock::now() < mState.PoolSlots[x].Lockout)
            continue;

        LotItem(x);
        LotCount++;
    }

    if (LotCount == 0)
        pOutput->message("There were no valid items to lot.");
    else if (LotCount == 1)
        pOutput->message_f("Sending lot packet for $H%d$R item.", LotCount);
    else
        pOutput->message_f("Sending lot packets for $H%d$R items.", LotCount);
}
void Lootwhore::HandleCommandPass(std::vector<string> args, int argcount, CommandHelp help)
{
    int PassCount = 0;

    for (int x = 0; x < 10; x++)
    {
        if (mState.PoolSlots[x].Id == 0)
            continue;
        if (mState.PoolSlots[x].Status != LotState::Untouched)
            continue;
        if (mState.PoolSlots[x].PacketAttempts >= mSettings.MaxRetry)
            continue;
        if (std::chrono::steady_clock::now() < mState.PoolSlots[x].Lockout)
            continue;

        PassItem(x);
        PassCount++;
    }

    if (PassCount == 0)
        pOutput->message("There were no valid items to pass.");
    else if (PassCount == 1)
        pOutput->message_f("Sending pass packet for $H%d$R item.", PassCount);
    else
        pOutput->message_f("Sending pass packets for $H%d$R items.", PassCount);
}
void Lootwhore::HandleCommandHelp(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount > 2)
    {
        auto iter = mCommandMap.find(args[2]);
        if (iter != mCommandMap.end())
        {
            PrintHelpText(iter->second.help, true);
            return;
        }
    }

    pOutput->message("Command List");
    for (auto iter = mCommandMap.begin(); iter != mCommandMap.end(); iter++)
    {
        PrintHelpText(iter->second.help, false);
    }
}

void Lootwhore::HandleCommandSearch(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    std::string searchTerm = "";
    for (int i = 2; i < argcount; i++)
    {
        if (i > 2)
            searchTerm += " ";
        searchTerm += args[i];
    }
    int resultCount = 0;
    int maxResults  = 10;

    pOutput->message_f("Search results for '$H%s$R':", searchTerm.c_str());

    for (uint16_t id = 1; id < 65535; id++)
    {
        IItem* item = m_AshitaCore->GetResourceManager()->GetItemById(id);
        if (!item || !item->Name[0] || strlen(item->Name[0]) < 1)
            continue;

        std::string itemName(item->Name[0]);
        itemName.erase(std::remove(itemName.begin(), itemName.end(), '"'), itemName.end());

        std::string search(searchTerm);
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
        std::transform(search.begin(), search.end(), search.begin(), ::tolower);
        if (itemName.find(search) != std::string::npos)
        {
            pOutput->message_f("[$H%d$R] %s", item->Id, item->Name[0]);
            resultCount++;
            if (resultCount >= maxResults)
            {
                pOutput->message_f("Reached limit of %d results. Narrow your search.", maxResults);
                break;
            }
        }
    }

    if (resultCount == 0)
        pOutput->message_f("No items found matching '$H%s$R'.", searchTerm.c_str());
}

void Lootwhore::HandleCommandAddStackIgnore(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    IItem* item = NULL;
    if (IsPositiveInteger(args[2]))
        item = m_AshitaCore->GetResourceManager()->GetItemById(atoi(args[2].c_str()));
    else
        item = m_AshitaCore->GetResourceManager()->GetItemByName(args[2].c_str(), 0);

    if ((item == NULL) || (strlen(item->Name[0]) < 3))
    {
        PrintHelpText(help, true);
        return;
    }

    mProfile.AutoStackIgnore.push_back((uint16_t)item->Id);
    pOutput->message_f("Added $H%s$R to AutoStack Ignore list.", item->Name[0]);
}

void Lootwhore::HandleCommandRemoveStackIgnore(std::vector<string> args, int argcount, CommandHelp help)
{
    if (argcount < 3)
    {
        PrintHelpText(help, true);
        return;
    }

    uint16_t id = 0;
    if (IsPositiveInteger(args[2]))
    {
        int tempId = atoi(args[2].c_str());
        if (tempId < 65535)
            id = (uint16_t)tempId;
    }

    int EraseCount = 0;

    for (std::list<uint16_t>::iterator iter = mProfile.AutoStackIgnore.begin(); iter != mProfile.AutoStackIgnore.end();)
    {
        if (id == *iter)
        {
            iter = mProfile.AutoStackIgnore.erase(iter);
            EraseCount++;
        }
        else
        {
            IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(*iter);
            if (_stricmp(pResource->Name[0], args[2].c_str()) == 0)
            {
                iter = mProfile.AutoStackIgnore.erase(iter);
                EraseCount++;
            }
            else
                iter++;
        }
    }

    if (EraseCount == 0)
        pOutput->error_f("Could not find a matching item to remove from AutoStack Ignore list.  [$H%s$R]", args[2].c_str());
    else if (EraseCount == 1)
        pOutput->message_f("$H%s$R removed from AutoStack Ignore list.", args[2].c_str());
    else
        pOutput->message_f("$H%d$R items matching $H%s$R were removed from AutoStack Ignore list.", EraseCount, args[2].c_str());
}

void Lootwhore::PrintHelpText(CommandHelp help, bool description)
{
    pOutput->message_f("$H%s", help.command.c_str());
    if (description)
        pOutput->message(help.description);
}