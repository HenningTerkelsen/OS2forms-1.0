<?php
define('DEFAULT_COUNTRY', 'DANMARK');
define('DEFAULT_EMAIL', 'WEBTEST@bellcom.dk');
define('DEFAULT_PASSWORD', 'retmig');
define('DEFAULT_OFFER_BY_EMAIL', 0);
define('DEFAULT_COMPANY_NO', 99);
define('DEFAULT_COMPANY_NO_PAY_IN', 9);
define('DEFAULT_NO_OF_YEARS', 1);
define('DEFAULT_STATUS', 0);
define('DEFAULT_TENANCY_TYPE', 1);
define('DEFAULT_TYPE_UPDATE', 'I');
define('DEFAULT_TYPE_CREATE', 'A');

/**
 * Gets a member object from the webservice based on CPR
 *
 * @return member object, or null if no member is found
 */
function egbolig_ws_get_member_by_cpr($cpr) {
  $client = new SoapClient(variable_get('soap_webservice'));
  $response = $client->MemberGetListByCprNo(
    array (
      'cprNo' => $cpr
    )
  );
  $result = $response->MemberGetListByCprNoResult;
  if (!empty((array) $result)) {
    return $result->Member;
  }
  return null;
}

/**
 * Creates a member object via the webservice;
 */
function egbolig_ws_create_member($member) {
  $member->Type = DEFAULT_TYPE_CREATE;

  $client = new SoapClient(variable_get('soap_webservice'));
  $response = $client->PaymentNewMember(
    [
      'paymentDetails' => [
        'NewAmounts' => [
          'Payment.NewAmount' => [
            'CompanyNo' => DEFAULT_COMPANY_NO,
            'CompanyNoToPayIn' => DEFAULT_COMPANY_NO_PAY_IN,
            'NoOfYears' => DEFAULT_NO_OF_YEARS,
            'Status' => DEFAULT_STATUS,
            'TenancyTypes' => [DEFAULT_TENANCY_TYPE]
          ]
        ]
      ],
      'member' => $member,
    ]
  );
}

/**
  * Updates the member via the webservice.
  * Internally calls _egbolig_ws_update_education
  */
function egbolig_ws_update_member($member) {
  $member->Type = DEFAULT_TYPE_UPDATE;
  $member_array = (array) $member;
  //unsetting the EdgiNo, this sabotages the submission
  unset($member_array['EgdiNo']);

  $client = new SoapClient(variable_get('soap_webservice'));

  $response = $client->MemberUpdate(
    array (
      'member' => $member_array,
    )
  );

  _egbolig_ws_update_education($member_array);
}

/**
 * Populates the member object with the values from the form submission.
 * Note: $member param is nullable. In this case a new member class instance is created with the default settings.
 *
 * @return populated member object
 */
function egbolig_populate_member($member, $form_state) {
  $v = $form_state['values']['submitted'];
  $tree = $form_state['webform']['component_tree']['children'];

  //check if member is empty, create a new member and populate some default fields
  if ($member == null) {
    $member = new stdClass();
    //TODO: check default fields
    $member->Email = DEFAULT_EMAIL;
    $member->Password = DEFAULT_PASSWORD;
    $member->OfferByEmail = DEFAULT_OFFER_BY_EMAIL;
    $member->CprNo = $v[_egbolig_get_webform_cid($tree, 'cpr')];
  }
  $member->Name = $v[_egbolig_get_webform_cid($tree, 'name')];
  $member->Address = $v[_egbolig_get_webform_cid($tree, 'addresse')];
  $member->HomePhone = $v[_egbolig_get_webform_cid($tree, 'phone_nr')];
  $member->CoAddress = $v[_egbolig_get_webform_cid($tree, 'c_o_addresse')];
  $member->MobilePhone = $v[_egbolig_get_webform_cid($tree, 'telefonnummer_mobil')];
  $member->PostalCodeCity = $v[_egbolig_get_webform_cid($tree, 'post_nr___by')];
  $member->Country = DEFAULT_COUNTRY;
  $member->Adults = 1 + $v[_egbolig_get_webform_cid($tree, 'has_partner')][0];//with or without the partner
  $children = 0;
  if ($v[_egbolig_get_webform_cid($tree, 'has_children')][0]) {//Børn over eller under 23 år i husstanden
    if ($v[_egbolig_get_webform_cid($tree, 'child_cpr_nr')]
        || $v[_egbolig_get_webform_cid($tree, 'cpr_nr')]
        || $v[_egbolig_get_webform_cid($tree, 'child_birthday')]) {
      $children++;
    }
    //counting adult child
    // if ($v[_egbolig_get_webform_cid($tree, 'adult_cpr_nr')]
    //     || $v[_egbolig_get_webform_cid($tree, 'adult_birthday')]) {
    //       $children++;
    // }
  }
  $member->Children = $children;
  $member->Education = $v[_egbolig_get_webform_cid($tree, 'education_name')];
  $member->EducationStart = $v[_egbolig_get_webform_cid($tree, 'education_start_date')];
  $member->EducationEnd = $v[_egbolig_get_webform_cid($tree, 'education_end_date')];
  $member->EducationExpectedEnd = $v[_egbolig_get_webform_cid($tree, 'education_end_date')];
  $member->EducationDocumentationReceived = ($v[_egbolig_get_webform_cid($tree, 'education_doc')] > 0);//TODO: not working with WS
  $member->EducationalInstitution = $v[_egbolig_get_webform_cid($tree, 'institution_name')];
  $member->MinimumRent = $v[_egbolig_get_webform_cid($tree, 'husleje_min')];
  $member->MaximumRent = $v[_egbolig_get_webform_cid($tree, 'husleje_max')];
  //$member->Pet TODO: fix the Pets list

  return $member;
}

/**
 * Returns a list of educations from webservice
 *
 * @return array of educations
 */
function education_get_list() {
  $client = new SoapClient(variable_get('soap_webservice'));
  $educations = $client->EducationGetList()->EducationGetListResult->Education;
  $edulist = array();

  foreach ($educations as $education) {
    if (strlen($education->Name) == 1) {
      continue;
    }
    elseif ($education->Name == 'æ' || $education->Name == 'ø' || $education->Name == 'å') {
      continue;
    }
    else {
      $edulist[$education->Id] = mb_convert_case($education->Name, MB_CASE_TITLE, "UTF-8");
    }
  }

  return $edulist;
}

/**
  * Updates the member education via the webservice.
  *
  */
function _egbolig_ws_update_education($member) {
  $client = new SoapClient(variable_get('soap_webservice'));
  $response = $client->MemberUpdateEducation(
    array (
      'member' => $member,
    )
  );
}

/**
 * Returns a list of education places from webservice
 *
 * @return array of education places
 */
function education_places() {
  $placeslist = array();
  $client = new SoapClient(variable_get('soap_webservice'));
  $education_places =
    $client->EducationalInstitutionGetList()->
    EducationalInstitutionGetListResult->
    EducationalInstitution;
  if(count($education_places) > 0) {
    foreach($education_places as $place)
    {
        $placeslist[$place->Id] = $place->Name;
    }
  }
  else {
    $placeslist[] = 'Ingen uddannelsessteder';
  }
  return $placeslist;
}

/**
 * Returns a list of accommodations from webservice
 *
 * @return array of accommodations
 */
function accommodation_types() {
  $accommodationlist = array();
  $client = new SoapClient(variable_get('soap_webservice'));
  $apartment_types =
    $client->TenancyTypeGetList()->
    TenancyTypeGetListResult->
    TenancyType;
  foreach($apartment_types as $type) {
      $accommodationlist[] = $type->Text;
  }
  return $accommodationlist;
}

function _egbolig_get_webform_cid($tree, $form_key) {
  foreach ($tree as $child) {
    if ($child['form_key'] === $form_key) {
      return $child['cid'];
    } else {
      if ($child['children']) {
        $cid = _egbolig_get_webform_cid($child['children'], $form_key);
        if ($cid) {
          return $cid;
        }
      }
    }
  }
}

/**
 * Returns a list of professions from webservice
 *
 * @return array of professions
 */
function profession_get_list() {
  $professionList = array();
  $client = new SoapClient(variable_get('soap_webservice'));
  $professions = $client->ProfessionGetList()->ProfessionGetListResult->Profession;
  foreach($professions as $prof) {
      $professionList[$prof->Id] = $prof->Name;
  }
  return $professionList;
}
