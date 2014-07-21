'use strict';

/* Directives */

var directives = angular.module('myApp.directives', []);

directives.

directive('appVersion', ['version', function(version) {
    return function(scope, elm, attrs) {
        elm.text(version);
    };
}]).

directive('autoFillSync', function($timeout) {
    return {
        require: 'ngModel',
        link: function(scope, elem, attrs, ngModel) {
            var origVal = elem.val();
            $timeout(function () {
                var newVal = elem.val();
                if(ngModel.$pristine && origVal !== newVal) {
                    ngModel.$setViewValue(newVal);
                }
            }, 500);
        }
    }
});
