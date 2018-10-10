/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/webui/brave_rewards_panel_ui.h"

#include "chrome/browser/braveRewards/brave_rewards_service.h"
#include "chrome/browser/braveRewards/brave_rewards_service_factory.h"

#include "chrome/browser/profiles/profile.h"
#include "chrome/common/webui_url_constants.h"
#include "components/grit/components_resources.h"
#include "components/grit/components_scaled_resources.h"
#include "components/strings/grit/components_chromium_strings.h"
#include "components/strings/grit/components_strings.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/browser/web_ui_message_handler.h"

using content::WebContents;
using content::WebUIMessageHandler;

namespace {

content::WebUIDataSource* CreateBraveRewardsPanelUIHTMLSource() {
  content::WebUIDataSource* source =
      content::WebUIDataSource::Create(chrome::kBraveRewardsPanelHost);

  source->AddResourcePath("brave_rewards_panel.bundle.js", IDR_BRAVE_REWARDS_PANEL_UI_JS);
  source->SetDefaultResource(IDR_BRAVE_REWARDS_PANEL_UI_HTML);
  source->UseGzip();
  return source;
}
////////////////////////////////////////////////////////////////////////////////
//
// RewardsPanelDOMHandler
//
////////////////////////////////////////////////////////////////////////////////

// The handler for Javascript messages for the about:flags page.
class RewardsPanelDOMHandler : public WebUIMessageHandler {
 public:
  RewardsPanelDOMHandler() {
  }
  ~RewardsPanelDOMHandler() override {}

  void Init();

  // WebUIMessageHandler implementation.
  void RegisterMessages() override;

private:
  // Callback for the "createWalletRequested" message.
  void HandleCreateWalletRequested(const base::ListValue* args);

  void HandleSetPublisherMinVisitTime(const base::ListValue* args);
  void HandleGetPublisherMinVisitTime(const base::ListValue* args);

  void HandleSetPublisherMinVisits(const base::ListValue* args);
  void HandleGetPublisherMinVisits(const base::ListValue* args);

  void HandleSetPublisherAllowNonVerified(const base::ListValue* args);
  void HandleGetPublisherAllowNonVerified(const base::ListValue* args);

  void HandleSetContributionAmount(const base::ListValue* args);
  void HandleGetContributionAmount(const base::ListValue* args);

  brave_rewards::BraveRewardsService* rewards_service_;

  DISALLOW_COPY_AND_ASSIGN(RewardsPanelDOMHandler);
};

void RewardsPanelDOMHandler::Init() {
  Profile* profile = Profile::FromWebUI(web_ui());
  rewards_service_ =
      BraveRewardsServiceFactory::GetForProfile(profile);
  // TODO add observer
  //if (rewards_service_)
  //  rewards_service_->AddObserver(this);
}

void RewardsPanelDOMHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "createWalletRequested",
      base::BindRepeating(&RewardsPanelDOMHandler::HandleCreateWalletRequested,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "setpublisherminvisittime",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleSetPublisherMinVisitTime,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "getpublisherminvisittime",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleGetPublisherMinVisitTime,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "setpublisherminvisits",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleSetPublisherMinVisits,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "getpublisherminvisits",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleGetPublisherMinVisits,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "setpublisherallownonverified",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleSetPublisherAllowNonVerified,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "getpublisherallownonverified",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleGetPublisherAllowNonVerified,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "setcontributionamount",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleSetContributionAmount,
      base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
    "getcontributionamount",
    base::BindRepeating(&RewardsPanelDOMHandler::HandleGetContributionAmount,
      base::Unretained(this)));
}


void RewardsPanelDOMHandler::HandleCreateWalletRequested(
    const base::ListValue* args) {
	LOG(ERROR) << "!!!HandleCreateWallet1";

  if (rewards_service_) {
    rewards_service_->CreateWallet();
  }
  /*DCHECK(flags_storage_);
  DCHECK_EQ(2u, args->GetSize());
  if (args->GetSize() != 2)
    return;

  std::string entry_internal_name;
  std::string enable_str;
  if (!args->GetString(0, &entry_internal_name) ||
      !args->GetString(1, &enable_str))
    return;

  about_flags::SetFeatureEntryEnabled(flags_storage_.get(), entry_internal_name,
                                      enable_str == "true");*/
}


void RewardsPanelDOMHandler::HandleSetPublisherMinVisitTime(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleSetPublisherMinVisitTime";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    std::string value_str;
    uint64_t value = -1;
    if ( !args->GetString(0, &value_str) || !base::StringToUint64(value_str, &value)) {
      LOG(ERROR) << "Failed to extract HandleSetPublisherMinVisitTime value.";
      return;
    }
    else{
      brave_rewards_service->SetPublisherMinVisitTime(value);
    }
  }
}

void RewardsPanelDOMHandler::HandleGetPublisherMinVisitTime(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleGetPublisherMinVisitTime";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    uint64_t value = brave_rewards_service->GetPublisherMinVisitTime();

    //base::Value doesn't accept uint64_t type
    base::Value v( (int)value);
    web_ui()->CallJavascriptFunctionUnsafe("returnPublisherMinVisitTime", v);
  }
}

void RewardsPanelDOMHandler::HandleSetPublisherMinVisits(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleSetPublisherMinVisits";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    std::string value_str;
    unsigned int value = -1;
    if (!args->GetString(0, &value_str) || !base::StringToUint(value_str, &value)) {
      LOG(ERROR) << "Failed to extract HandleSetPublisherMinVisits value.";
      return;
    }
    else {
      brave_rewards_service->SetPublisherMinVisits(value);
    }
  }
}

void RewardsPanelDOMHandler::HandleGetPublisherMinVisits(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleGetPublisherMinVisits";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    unsigned int value = brave_rewards_service->GetPublisherMinVisits();

    //base::Value doesn't accept unsigned int type
    base::Value v((int)value);
    web_ui()->CallJavascriptFunctionUnsafe("returnPublisherMinVisits", v);
  }
}

void RewardsPanelDOMHandler::HandleSetPublisherAllowNonVerified(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleSetPublisherAllowNonVerified";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    std::string value_str;
    if (!args->GetString(0, &value_str) || (value_str != "true" && value_str != "false") ) {
      LOG(ERROR) << "Failed to extract HandleSetPublisherAllowNonVerified value.";
      return;
    }
    else {
      brave_rewards_service->SetPublisherAllowNonVerified(value_str == "true" ? true : false);
    }
  }
}

void RewardsPanelDOMHandler::HandleGetPublisherAllowNonVerified(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleGetPublisherAllowNonVerified";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    bool value = brave_rewards_service->GetPublisherAllowNonVerified();
    base::Value v(value);
    web_ui()->CallJavascriptFunctionUnsafe("returnPublisherAllowNonVerified", v);
  }
}

void RewardsPanelDOMHandler::HandleSetContributionAmount(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleSetContributionAmount";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    std::string value_str;
    double value = -1.0;
    if (!args->GetString(0, &value_str) || !base::StringToDouble(value_str, &value)) {
      LOG(ERROR) << "Failed to extract HandleSetContributionAmount value.";
      return;
    }
    else {
      brave_rewards_service->SetContributionAmount(value);
    }
  }
}

void RewardsPanelDOMHandler::HandleGetContributionAmount(
  const base::ListValue* args) {
  LOG(ERROR) << "!!!HandleGetContributionAmount";

  Profile* profile = Profile::FromWebUI(web_ui());
  brave_rewards::BraveRewardsService* brave_rewards_service =
    BraveRewardsServiceFactory::GetForProfile(profile);

  if (brave_rewards_service) {
    double value = brave_rewards_service->GetContributionAmount();
    base::Value v(value);
    web_ui()->CallJavascriptFunctionUnsafe("returnContributionAmount", v);
  }
}

}  // namespace

///////////////////////////////////////////////////////////////////////////////
//
// BraveRewardsPanelUI
//
///////////////////////////////////////////////////////////////////////////////

BraveRewardsPanelUI::BraveRewardsPanelUI(content::WebUI* web_ui)
    : WebUIController(web_ui),
      weak_factory_(this) {
  Profile* profile = Profile::FromWebUI(web_ui);

  auto handler_owner = std::make_unique<RewardsPanelDOMHandler>();
  RewardsPanelDOMHandler* handler = handler_owner.get();
  web_ui->AddMessageHandler(std::move(handler_owner));
  handler->Init();

  // Set up the brave://rewards-panel source.
  content::WebUIDataSource::Add(profile, CreateBraveRewardsPanelUIHTMLSource());
}

BraveRewardsPanelUI::~BraveRewardsPanelUI() {
}
