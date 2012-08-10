/*
 * Subject.cpp
 *
 *  Created on: 2012-8-8
 *      Author: chenhl
 */


#include <cetty/shiro/subject/Subject.h>

namespace cetty {
namespace shiro {
namespace subject {
std::string Subject::RUN_AS_PRINCIPALS_SESSION_KEY = "cetty.shiro.subject.Subject.RUN_AS_PRINCIPALS_SESSION_KEY";

bool Subject::login(const AuthenticationToken &token){
    clearRunAsIdentities();
    Subject subject = securityManager.login(this, token);

          PrincipalCollection principals;

          String host = null;

          if (subject instanceof DelegatingSubject) {
              DelegatingSubject delegating = (DelegatingSubject) subject;
              //we have to do this in case there are assumed identities - we don't want to lose the 'real' principals:
              principals = delegating.principals;
              host = delegating.host;
          } else {
              principals = subject.getPrincipals();
          }

          if (principals == null || principals.isEmpty()) {
              String msg = "Principals returned from securityManager.login( token ) returned a null or " +
                      "empty value.  This value must be non null and populated with one or more elements.";
              throw new IllegalStateException(msg);
          }
          this.principals = principals;
          this.authenticated = true;
          if (token instanceof HostAuthenticationToken) {
              host = ((HostAuthenticationToken) token).getHost();
          }
          if (host != null) {
              this.host = host;
          }
          Session session = subject.getSession(false);
          if (session != null) {
              this.session = decorate(session);
              this.runAsPrincipals = getRunAsPrincipals(this.session);
          } else {
              this.session = null;
          }
          ThreadContext.bind(this);
}
}
}
}

